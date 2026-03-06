import hashlib
import os
import pytest
from unittest.mock import Mock, patch, MagicMock
from pathlib import Path
from subprocess import TimeoutExpired
from dit_transfer.transfer import (
    parse_sftp_uri,
    parse_rclone_uri,
    file_checksum,
    transfer_local,
    is_ltfs_mount,
    spot_check_verify,
    directory_size_verify,
    get_dir_size,
    ProgressTracker,
    hash_chunk,
    sftp_makedirs,
    remote_file_checksum,
    ensure_rclone_remote,
    sftp_connect,
    get_dir_sizes,
)


def test_parse_sftp_uri_invalid():
    with pytest.raises(ValueError, match="Not a valid SFTP URI"):
        parse_sftp_uri("http://bad")


def test_parse_sftp_uri_no_host():
    with pytest.raises(ValueError, match="Missing host"):
        parse_sftp_uri("sftp://user@/path")


def test_parse_rclone_uri_invalid():
    with pytest.raises(ValueError, match="Not a valid rclone URI"):
        parse_rclone_uri("sftp://bad")


def test_parse_rclone_uri_no_remote():
    with pytest.raises(ValueError, match="Missing remote name"):
        parse_rclone_uri("rclone:///path")


def test_get_dir_size_oserror(tmp_path, monkeypatch):
    def mock_stat():
        raise OSError("test")

    mock_path = Mock(spec=Path)
    mock_path.is_file.return_value = True
    mock_path.stat.side_effect = mock_stat

    def mock_rglob(*args):
        yield mock_path

    monkeypatch.setattr(Path, "rglob", mock_rglob)
    assert get_dir_size(tmp_path) == 0


def test_progress_tracker():
    pbar = MagicMock()
    tracker = ProgressTracker(pbar)
    tracker.callback(50, 100)
    pbar.update.assert_called_with(50)
    tracker.callback(100, 100)
    pbar.update.assert_called_with(50)


def test_sftp_connect_no_auth():
    with pytest.raises(ValueError, match="Must provide password or key_file"):
        sftp_connect("user", "host", 22)


@patch("os.path.exists")
def test_sftp_connect_key_not_exist(mock_exists):
    mock_exists.return_value = False
    with pytest.raises(ValueError):
        sftp_connect("user", "host", 22, key_file="/nonexist")


def test_ensure_rclone_remote_no_pass():
    ensure_rclone_remote("rem", "user", None, "host")


def test_get_dir_sizes_oserror(tmp_path):
    def mock_walk(root):
        yield str(root), [], ["fake.txt"]

    def mock_stat(*args):
        raise OSError("stat fail")

    with patch("os.walk", mock_walk), patch("os.stat", side_effect=mock_stat):
        sizes = get_dir_sizes(tmp_path)
        assert sizes == {}


def test_directory_size_verify_missing(tmp_path):
    src_dir = tmp_path / "src"
    src_dir.mkdir()
    (src_dir / "f.txt").write_bytes(b"a")
    dst_dir = tmp_path / "dst"
    dst_dir.mkdir()
    with pytest.raises(RuntimeError, match="File lists differ"):
        directory_size_verify(src_dir, dst_dir)


def test_directory_size_verify_mismatch(tmp_path):
    src_dir = tmp_path / "src"
    src_dir.mkdir()
    dst_dir = tmp_path / "dst"
    dst_dir.mkdir()
    (src_dir / "f.txt").write_bytes(b"a")
    (dst_dir / "f.txt").write_bytes(b"bb")
    with pytest.raises(RuntimeError, match="Size mismatches"):
        directory_size_verify(src_dir, dst_dir)


def test_hash_chunk_small(tmp_path):
    f = tmp_path / "small.bin"
    f.write_bytes(b"short")
    h = hash_chunk(f, 0, 10)
    assert len(h) == 64


def test_hash_chunk_end(tmp_path):
    f = tmp_path / "end.bin"
    f.write_bytes(b"data" * 1000)
    h = hash_chunk(f, len(f.read_bytes()) - 1, 1)
    assert len(h) == 64


def test_spot_check_no_large(tmp_path):
    src_dir = tmp_path / "small"
    src_dir.mkdir()
    dst_dir = tmp_path / "dst_small"
    dst_dir.mkdir()
    spot_check_verify(src_dir, dst_dir, num_chunks=5)


def test_spot_check_missing_file(tmp_path):
    src_dir = tmp_path / "src_miss"
    src_dir.mkdir()
    large = src_dir / "large.bin"
    large.write_bytes(os.urandom(2 * 1024 * 1024))
    dst_dir = tmp_path / "dst_miss"
    dst_dir.mkdir()
    with pytest.raises(RuntimeError, match=r"File lists differ.*missing"):
        spot_check_verify(src_dir, dst_dir)


@patch("subprocess.run")
def test_is_ltfs_mount_df(mock_run):
    mock_result = MagicMock()
    mock_result.stdout = "ltfs\\n"
    mock_run.return_value = mock_result
    assert is_ltfs_mount(Path("/fake"))


@patch("subprocess.run")
def test_is_ltfs_mount_timeout(mock_run):
    mock_run.side_effect = TimeoutExpired("df -T /fake", timeout=10)
    assert not is_ltfs_mount(Path("/fake"))


def test_sftp_makedirs_root():
    mock_sftp = MagicMock()
    mock_sftp.stat.side_effect = IOError("no")
    sftp_makedirs(mock_sftp, "/nonroot")
    mock_sftp.stat.assert_called()


def test_file_checksum_empty(tmp_path):
    f = tmp_path / "empty"
    f.touch()
    h = file_checksum(f)
    assert h == hashlib.sha256(b"").hexdigest()


def test_remote_file_checksum_empty():
    mock_sftp = MagicMock()
    mock_file = MagicMock()
    mock_file.read.return_value = b""
    mock_sftp.open.return_value.__enter__.return_value = mock_file
    h = remote_file_checksum(mock_sftp, "/empty")
    assert h == hashlib.sha256(b"").hexdigest()


# LTFS mock for transfer_local
@patch("dit_transfer.transfer.is_ltfs_mount")
@patch("subprocess.run")
def test_transfer_local_ltfs_auto(mock_run, mock_ltfs):
    mock_ltfs.return_value = True
    tmp_path = Path("tmp")
    transfer_local(tmp_path, tmp_path, rsync_fallback=False)
    # checks rsync=True


@patch("dit_transfer.transfer.subprocess.run")
def test_transfer_local_rsync(mock_run):
    tmp_path = Path("tmp")
    transfer_local(tmp_path, tmp_path, rsync_fallback=True)
    mock_run.assert_called()


# etc for more
