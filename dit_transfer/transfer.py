import os
import stat
import hashlib
from pathlib import Path
from urllib.parse import urlparse
import paramiko
from tqdm import tqdm

class ProgressTracker:
    def __init__(self, pbar):
        self.pbar = pbar
        self.transferred = 0

    def callback(self, transferred, total):
        self.pbar.update(transferred - self.transferred)
        self.transferred = transferred

def parse_sftp_uri(uri: str) -&gt; tuple[str, str, int, str]:
    parsed = urlparse(uri)
    if parsed.scheme.lower() != &#x27;sftp&#x27;:
        raise ValueError(f&#x27;Not a valid SFTP URI: {uri}&#x27;)
    user = parsed.username or os.getenv(&#x27;USER&#x27;)
    host = parsed.hostname
    port = parsed.port or 22
    path = parsed.path.lstrip(&#x27;/&#x27;)
    if not host:
        raise ValueError(&#x27;Missing host in SFTP URI&#x27;)
    return user, host, int(port), path

def sftp_connect(user, host, port, password=None, key_file=None):
    client = paramiko.SSHClient()
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    if key_file and os.path.exists(key_file):
        pkey = paramiko.RSAKey.from_private_key_file(key_file)
        client.connect(hostname=host, port=port, username=user, pkey=pkey)
    elif password:
        client.connect(hostname=host, port=port, username=user, password=password)
    else:
        raise ValueError(&#x27;Must provide password or key_file&#x27;)
    sftp = client.open_sftp()
    return sftp, client

def sftp_makedirs(sftp, remote_path: str):
    if remote_path and remote_path != &#x27;/&#x27;:
        try:
            sftp.stat(remote_path)
        except IOError:
            parent = os.path.dirname(remote_path)
            if parent:
                sftp_makedirs(sftp, parent)
            sftp.mkdir(remote_path)

def file_checksum(path: Path, alg=&#x27;sha256&#x27;, chunk_size=65536):
    h = hashlib.new(alg)
    with open(path, &#x27;rb&#x27;) as f:
        for chunk in iter(lambda: f.read(chunk_size), b&#x27;&#x27;):
            h.update(chunk)
    return h.hexdigest()

def remote_file_checksum(sftp, remote_path: str, alg=&#x27;sha256&#x27;, chunk_size=65536):
    h = hashlib.new(alg)
    with sftp.open(remote_path, &#x27;rb&#x27;) as f:
        for chunk in iter(lambda: f.read(chunk_size), b&#x27;&#x27;):
            h.update(chunk)
    return h.hexdigest()

def transfer_local_to_sftp(local_path: Path, remote_base: str, sftp, verify=False, desc=&#x27;Uploading&#x27;):
    remote_base = remote_base.rstrip(&#x27;/&#x27;)
    if local_path.is_dir():
        total_size = sum(p.stat().st_size for p in local_path.rglob(&#x27;*&#x27;) if p.is_file())
        with tqdm(total=total_size, unit=&#x27;B&#x27;, unit_scale=True, desc=desc) as pbar:
            tracker = ProgressTracker(pbar)
            def upload_dir(local_dir: str, remote_dir: str):
                sftp_makedirs(sftp, remote_dir)
                for entry in os.scandir(local_dir):
                    if entry.is_dir(follow_symlinks=False):
                        upload_dir(entry.path, remote_dir + &#x27;/&#x27; + entry.name)
                    else:
                        remote_file = remote_dir + &#x27;/&#x27; + entry.name
                        tracker.transferred = 0
                        sftp.put(entry.path, remote_file, callback=tracker.callback)
                        if verify:
                            local_cs = file_checksum(Path(entry.path))
                            remote_cs = remote_file_checksum(sftp, remote_file)
                            if local_cs != remote_cs:
                                raise RuntimeError(f&#x27;Checksum mismatch for {entry.name}&#x27;)
            upload_dir(str(local_path), remote_base)
    else:
        total_size = local_path.stat().st_size
        remote_file = remote_base + &#x27;/&#x27; + local_path.name
        sftp_makedirs(sftp, remote_base)
        with tqdm(total=total_size, unit=&#x27;B&#x27;, unit_scale=True, desc=desc) as pbar:
            tracker = ProgressTracker(pbar)
            tracker.transferred = 0
            sftp.put(str(local_path), remote_file, callback=tracker.callback)
            if verify:
                local_cs = file_checksum(local_path)
                remote_cs = remote_file_checksum(sftp, remote_file)
                if local_cs != remote_cs:
                    raise RuntimeError(f&#x27;Checksum mismatch for {local_path.name}&#x27;)

def transfer_sftp_to_local(remote_base: str, local_base: Path, sftp, verify=False, desc=&#x27;Downloading&#x27;):
    try:
        st = sftp.stat(remote_base)
        is_dir = stat.S_ISDIR(st.st_mode)
    except FileNotFoundError:
        raise FileNotFoundError(f&#x27;Remote path not found: {remote_base}&#x27;)
    if is_dir:
        total_size = 0
        def rsize(rpath):
            nonlocal total_size
            attrs = sftp.listdir_attr(rpath)
            for attr in attrs:
                fpath = rpath + &#x27;/&#x27; + attr.filename if rpath else attr.filename
                if stat.S_ISDIR(attr.st_mode):
                    rsize(fpath)
                elif stat.S_ISREG(attr.st_mode):
                    total_size += attr.st_size
        rsize(remote_base)
        with tqdm(total=total_size, unit=&#x27;B&#x27;, unit_scale=True, desc=desc) as pbar:
            tracker = ProgressTracker(pbar)
            def download_dir(remote_dir: str, local_dir: Path):
                local_dir.mkdir(parents=True, exist_ok=True)
                attrs = sftp.listdir_attr(remote_dir)
                for attr in attrs:
                    rfile = remote_dir + &#x27;/&#x27; + attr.filename
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
                                raise RuntimeError(f&#x27;Checksum mismatch for {attr.filename}&#x27;)
            download_dir(remote_base, local_base)
    else:
        st = sftp.stat(remote_base)
        total_size = st.st_size
        local_file = local_base / Path(remote_base).name
        local_file.parent.mkdir(parents=True, exist_ok=True)
        with tqdm(total=total_size, unit=&#x27;B&#x27;, unit_scale=True, desc=desc) as pbar:
            tracker = ProgressTracker(pbar)
            tracker.transferred = 0
            sftp.get(remote_base, str(local_file), callback=tracker.callback)
            if verify:
                remote_cs = remote_file_checksum(sftp, remote_base)
                local_cs = file_checksum(local_file)
                if local_cs != remote_cs:
                    raise RuntimeError(&#x27;Checksum mismatch&#x27;)

def transfer_local(src: Path, dst: Path, verify=False):
    import shutil
    if src.is_dir():
        shutil.copytree(src, dst, dirs_exist_ok=True, copy_function=shutil.copy2)
    else:
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(src, dst)
    if verify:
        if src.is_dir():
            for sfile in src.rglob(&#x27;*&#x27;):
                if sfile.is_file():
                    dfile = dst / sfile.relative_to(src)
                    if file_checksum(sfile) != file_checksum(dfile):
                        raise RuntimeError(f&#x27;Mismatch {sfile}&#x27;)
        else:
            if file_checksum(src) != file_checksum(dst):
                raise RuntimeError(&#x27;Mismatch&#x27;)
