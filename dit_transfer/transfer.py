import os
import stat
import hashlib
from pathlib import Path
from urllib.parse import urlparse
import paramiko  # type: ignore[import-untyped]
from tqdm import tqdm  # type: ignore[import-untyped]
import subprocess
import configparser
import uuid


def get_dir_size(src_dir: Path) -> int:
    total = 0
    for p in src_dir.rglob("*"):
        try:
            if p.is_file():
                total += p.stat().st_size
        except OSError:
            continue
    return total


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
    user = parsed.username or os.getenv("USER") or ""
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


def get_dir_sizes(path: Path) -> dict[str, int]:
    sizes = {}
    for r, dirs, files in os.walk(path):
        rel_root = os.path.relpath(r, str(path))
        for f in files:
            fpath = os.path.join(r, f)
            try:
                st = os.stat(fpath)
                rel = os.path.join(rel_root, f) if rel_root != "." else f
                sizes[rel] = st.st_size
            except OSError:
                pass
    return sizes


def directory_size_verify(src: Path, dst: Path) -> bool:
    src_sizes = get_dir_sizes(src)
    dst_sizes = get_dir_sizes(dst)
    src_files = set(src_sizes.keys())
    dst_files = set(dst_sizes.keys())
    if src_files != dst_files:
        missing = src_files - dst_files
        extra = dst_files - src_files
        raise RuntimeError(
            f"File lists differ: missing {sorted(list(missing))}, extra {sorted(list(extra))}"
        )
    mismatches = [f for f in src_files if src_sizes[f] != dst_sizes[f]]
    if mismatches:
        raise RuntimeError(
            f"Size mismatches in {len(mismatches)} files: {mismatches[:5]}"
        )
    return True


def hash_chunk(path: Path, offset: int, size: int) -> str:
    h = hashlib.sha256()
    with open(path, "rb") as f:
        f.seek(offset)
        remaining = size
        while remaining > 0:
            read_size = min(1024 * 1024, remaining)
            chunk = f.read(read_size)
            if not chunk:
                break
            h.update(chunk)
            remaining -= len(chunk)
    return h.hexdigest()


def spot_check_verify(
    src: Path, dst: Path, num_chunks: int = 5, chunk_mb: int = 1
) -> None:
    chunk_size = chunk_mb * 1024 * 1024
    directory_size_verify(src, dst)
    large_files = []
    try:
        for f in src.rglob("*"):
            if f.is_file():
                try:
                    if f.stat().st_size >= chunk_size:
                        large_files.append(f)
                        if len(large_files) >= num_chunks * 20:
                            break
                except Exception:
                    continue
    except Exception:
        large_files = []  # too large dir
    import random

    if len(large_files) > num_chunks:
        large_files = random.sample(large_files, num_chunks)
    num_to_check = min(num_chunks, len(large_files))
    for i in range(num_to_check):
        f_src = large_files[i]
        rel = f_src.relative_to(src)
        f_dst = dst / rel
        if not f_dst.exists():
            raise RuntimeError(f"Missing file after transfer: {rel}")
        if f_src.stat().st_size != f_dst.stat().st_size:
            raise RuntimeError(f"Size mismatch: {rel}")
        offset = random.randint(0, f_src.stat().st_size - chunk_size)
        if hash_chunk(f_src, offset, chunk_size) != hash_chunk(
            f_dst, offset, chunk_size
        ):
            raise RuntimeError(f"Spot-check chunk mismatch: {rel} offset={offset}")
    print(f"✓ Spot-check verified {num_to_check}/{num_chunks} random chunks OK.")


def is_ltfs_mount(path: Path) -> bool:
    try:
        result = subprocess.run(
            ["df", "-T", str(path)], capture_output=True, text=True, timeout=10
        )
        if "ltfs" in result.stdout.lower() or "ltfs" in result.stderr.lower():
            return True
    except Exception:
        pass
    if str(path).startswith("/Volumes/LTO"):
        return True
    try:
        result = subprocess.run(["mount"], capture_output=True, text=True, timeout=10)
        mount_info = result.stdout
        if "ltfs" in mount_info and str(path) in mount_info:
            return True
    except Exception:
        pass
    return False


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


def copy_file(
    src_path: str, dst_path: str, callback, buf_size: int = 64 * 1024
) -> None:
    stat_src = os.stat(src_path)
    total_size = stat_src.st_size
    transferred = 0
    os.makedirs(os.path.dirname(dst_path), exist_ok=True)
    buf_size = buf_size
    with open(src_path, "rb") as fsrc, open(dst_path, "wb") as fdst:
        while True:
            chunk = fsrc.read(buf_size)
            if not chunk:
                break
            fdst.write(chunk)
            transferred += len(chunk)
            callback(transferred, total_size)
    os.utime(dst_path, ns=(stat_src.st_atime_ns, stat_src.st_mtime_ns))
    os.chmod(dst_path, stat_src.st_mode)


def transfer_local(
    src: Path,
    dst: Path,
    verify=False,
    rsync_fallback=False,
    spot_check=False,
    num_chunks=5,
    chunk_mb=1,
    buffer_size: int = 64 * 1024 * 1024,
    sequential_flags: bool = False,
):
    copy_dst = dst / src.name if dst.exists() and dst.is_dir() else dst

    is_ltfs_src = is_ltfs_mount(src)
    is_ltfs_dst = is_ltfs_mount(dst)
    is_ltfs = is_ltfs_src or is_ltfs_dst
    rsync_fallback = rsync_fallback or is_ltfs_dst  # Auto rsync fallback for LTFS destinations
    do_spot_check = spot_check or (verify and is_ltfs) or sequential_flags

    if rsync_fallback:
        src_str = f"{str(src)}/" if src.is_dir() else str(src)
        dst_str = f"{str(copy_dst)}/" if src.is_dir() else str(copy_dst)
        flags = ["-aHXS", "--whole-file", "--progress"]
        if sequential_flags:
            flags.append("--inplace")
        cmd = ["rsync"] + flags + [src_str, dst_str]
        subprocess.run(cmd, check=True)
    else:
        if src.is_file():
            copy_dst.parent.mkdir(parents=True, exist_ok=True)
            total_size = src.stat().st_size
            with tqdm(
                total=total_size, unit="B", unit_scale=True, desc=f"Copying {src.name}"
            ) as pbar:
                tracker = ProgressTracker(pbar)
                copy_file(str(src), str(copy_dst), tracker.callback, buffer_size)
        else:
            total_size = get_dir_size(src)
            with tqdm(
                total=total_size, unit="B", unit_scale=True, desc="Local transfer"
            ) as pbar:

                def copy_tree(curr_src: Path, curr_dst: Path, buffer_size: int, pbar):
                    curr_dst.mkdir(parents=True, exist_ok=True)
                    for item in curr_src.iterdir():
                        if item.is_symlink():
                            continue
                        if item.is_dir():
                            copy_tree(item, curr_dst / item.name, buffer_size, pbar)
                        else:
                            tracker = ProgressTracker(pbar)
                            copy_file(
                                str(item),
                                str(curr_dst / item.name),
                                tracker.callback,
                                buffer_size,
                            )

                copy_tree(src, copy_dst, buffer_size, pbar)

    directory_size_verify(src, copy_dst)
    if do_spot_check:
        spot_check_verify(src, copy_dst, num_chunks, chunk_mb)


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
