import os
import stat
import hashlib
from pathlib import Path
from urllib.parse import urlparse
import paramiko
from tqdm import tqdm
import subprocess
import configparser
import uuid


class ProgressTracker:
    def __init__(self, pbar):
        self.pbar = pbar
        self.transferred = 0

    def callback(self, transferred, total):
        self.pbar.update(transferred - self.transferred)
        self.transferred = transferred


def parse_sftp_uri(uri: str) -> tuple[str, str, int, str]:
    parsed = urlparse(uri)
    if parsed.scheme.lower() != "sftp":
        raise ValueError(f"Not a valid SFTP URI: {uri}")
    user = parsed.username or os.getenv("USER")
    host = parsed.hostname
    port = parsed.port or 22
    path = parsed.path.lstrip("/")
    if not host:
        raise ValueError("Missing host in SFTP URI")
    return user, host, int(port), path


def parse_rclone_uri(uri: str) -> tuple[str, str, str | None, str | None, int, str]:
    parsed = urlparse(uri)
    if parsed.scheme.lower() != "rclone":
        raise ValueError(f"Not a valid rclone URI: {uri}")
    remote_name = parsed.hostname
    if not remote_name:
        raise ValueError("Missing remote name in rclone URI")
    user = parsed.username
    password = parsed.password
    port = parsed.port or 22
    path = parsed.path.lstrip("/")
    host = remote_name
    return remote_name, host, user, password, int(port), path


def sftp_connect(user, host, port, password=None, key_file=None):
    client = paramiko.SSHClient()
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    if key_file and os.path.exists(key_file):
        pkey = paramiko.RSAKey.from_private_key_file(key_file)
        client.connect(hostname=host, port=port, username=user, pkey=pkey)
    elif password:
        client.connect(hostname=host, port=port, username=user, password=password)
    else:
        raise ValueError("Must provide password or key_file")
    sftp = client.open_sftp()
    return sftp, client


def ensure_rclone_remote(
    remote_name: str, user: str, password: str | None, host: str, port: int = 22
):
    if not password:
        return
    conf_path = Path.home() / ".config" / "rclone" / "rclone.conf"
    conf_path.parent.mkdir(parents=True, exist_ok=True)
    config = configparser.ConfigParser(interpolation=None)
    config.read(conf_path)
    if remote_name in config:
        return
    obf_pass = (
        subprocess.check_output(["rclone", "obscure", password]).decode("utf-8").strip()
    )
    config[remote_name] = {
        "type": "sftp",
        "host": host,
        "user": user,
        "port": str(port),
        "pass": obf_pass,
    }
    with open(conf_path, "w") as f:
        config.write(f)


def sftp_makedirs(sftp, remote_path: str):
    if remote_path and remote_path != "/":
        try:
            sftp.stat(remote_path)
        except IOError:
            parent = os.path.dirname(remote_path)
            if parent:
                sftp_makedirs(sftp, parent)
            sftp.mkdir(remote_path)


def file_checksum(path: Path, alg="sha256", chunk_size=65536):
    h = hashlib.new(alg)
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(chunk_size), b""):
            h.update(chunk)
    return h.hexdigest()


def remote_file_checksum(sftp, remote_path: str, alg="sha256", chunk_size=65536):
    h = hashlib.new(alg)
    with sftp.open(remote_path, "rb") as f:
        for chunk in iter(lambda: f.read(chunk_size), b""):
            h.update(chunk)
    return h.hexdigest()


def transfer_local_to_sftp(
    local_path: Path, remote_base: str, sftp, verify=False, desc="Uploading"
):
    remote_base = remote_base.rstrip("/")
    if local_path.is_dir():
        total_size = sum(p.stat().st_size for p in local_path.rglob("*") if p.is_file())
        with tqdm(total=total_size, unit="B", unit_scale=True, desc=desc) as pbar:
            tracker = ProgressTracker(pbar)

            def upload_dir(local_dir: str, remote_dir: str):
                sftp_makedirs(sftp, remote_dir)
                for entry in os.scandir(local_dir):
                    if entry.is_dir(follow_symlinks=False):
                        upload_dir(entry.path, remote_dir + "/" + entry.name)
                    else:
                        remote_file = remote_dir + "/" + entry.name
                        tracker.transferred = 0
                        sftp.put(entry.path, remote_file, callback=tracker.callback)
                        if verify:
                            local_cs = file_checksum(Path(entry.path))
                            remote_cs = remote_file_checksum(sftp, remote_file)
                            if local_cs != remote_cs:
                                raise RuntimeError(
                                    f"Checksum mismatch for {entry.name}"
                                )

            upload_dir(str(local_path), remote_base)
    else:
        total_size = local_path.stat().st_size
        remote_file = remote_base + "/" + local_path.name
        sftp_makedirs(sftp, remote_base)
        with tqdm(total=total_size, unit="B", unit_scale=True, desc=desc) as pbar:
            tracker = ProgressTracker(pbar)
            tracker.transferred = 0
            sftp.put(str(local_path), remote_file, callback=tracker.callback)
            if verify:
                local_cs = file_checksum(local_path)
                remote_cs = remote_file_checksum(sftp, remote_file)
                if local_cs != remote_cs:
                    raise RuntimeError(f"Checksum mismatch for {local_path.name}")


def transfer_sftp_to_local(
    remote_base: str, local_base: Path, sftp, verify=False, desc="Downloading"
):
    try:
        st = sftp.stat(remote_base)
        is_dir = stat.S_ISDIR(st.st_mode)
    except FileNotFoundError:
        raise FileNotFoundError(f"Remote path not found: {remote_base}")
    if is_dir:
        total_size = 0

        def rsize(rpath):
            nonlocal total_size
            attrs = sftp.listdir_attr(rpath)
            for attr in attrs:
                fpath = rpath + "/" + attr.filename if rpath else attr.filename
                if stat.S_ISDIR(attr.st_mode):
                    rsize(fpath)
                elif stat.S_ISREG(attr.st_mode):
                    total_size += attr.st_size

        rsize(remote_base)
        with tqdm(total=total_size, unit="B", unit_scale=True, desc=desc) as pbar:
            tracker = ProgressTracker(pbar)

            def download_dir(remote_dir: str, local_dir: Path):
                local_dir.mkdir(parents=True, exist_ok=True)
                attrs = sftp.listdir_attr(remote_dir)
                for attr in attrs:
                    rfile = remote_dir + "/" + attr.filename
                    lfile = local_dir / attr.filename
                    if stat.S_ISDIR(attr.st_mode):
                        download_dir(rfile, lfile)
                    elif stat.S_ISREG(attr.st_mode):
                        tracker.transferred = 0
                        sftp.get(rfile, str(lfile), callback=tracker.callback)
                        if verify:
                            remote_cs = remote_file_checksum(sftp, rfile)
                            local_cs = file_checksum(lfile)
                            if local_cs != remote_cs:
                                raise RuntimeError(
                                    f"Checksum mismatch for {attr.filename}"
                                )

            download_dir(remote_base, local_base)
    else:
        st = sftp.stat(remote_base)
        total_size = st.st_size
        local_file = local_base / Path(remote_base).name
        local_file.parent.mkdir(parents=True, exist_ok=True)
        with tqdm(total=total_size, unit="B", unit_scale=True, desc=desc) as pbar:
            tracker = ProgressTracker(pbar)
            tracker.transferred = 0
            sftp.get(remote_base, str(local_file), callback=tracker.callback)
            if verify:
                remote_cs = remote_file_checksum(sftp, remote_base)
                local_cs = file_checksum(local_file)
                if local_cs != remote_cs:
                    raise RuntimeError("Checksum mismatch")


def transfer_local(src: Path, dst: Path, verify=False):
    import shutil

    if src.is_dir():
        shutil.copytree(src, dst, dirs_exist_ok=True, copy_function=shutil.copy2)
    else:
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(src, dst)
    if verify:
        if src.is_dir():
            for sfile in src.rglob("*"):
                if sfile.is_file():
                    dfile = dst / sfile.relative_to(src)
                    if file_checksum(sfile) != file_checksum(dfile):
                        raise RuntimeError(f"Mismatch {sfile}")
        else:
            if file_checksum(src) != file_checksum(dst):
                raise RuntimeError("Mismatch")


def transfer_with_rclone(
    src: str, dst: str, verify: bool = False, concurrency: int = 4
) -> None:
    cmd = [
        "rclone",
        "copy",
        src,
        dst,
        "--progress",
        "--checksum",
        "--transfers",
        str(concurrency),
    ]
    subprocess.run(cmd, check=True)
    if verify:
        check_cmd = ["rclone", "check", src, dst, "--download"]
        subprocess.run(check_cmd, check=True)


def create_temp_rclone_sftp_remote(
    uri: str, password: str | None = None
) -> tuple[str, str]:
    """Create temporary rclone SFTP remote from SFTP URI."""
    user, host, port, path = parse_sftp_uri(uri)
    remote_name = f"temp_sftp_{uuid.uuid4().hex[:8]}"
    ensure_rclone_remote(remote_name, user, password, host, port)
    return remote_name, path


def cleanup_temp_rclone_remote(remote_name: str):
    """Cleanup temporary rclone remote."""
    conf_path = Path.home() / ".config" / "rclone" / "rclone.conf"
    config = configparser.ConfigParser(interpolation=None)
    if conf_path.exists():
        config.read(str(conf_path))
        if remote_name in config:
            del config[remote_name]
            conf_path.parent.mkdir(parents=True, exist_ok=True)
            with open(conf_path, "w") as f:
                config.write(f)
