import hashlib
import os
import pytest
from unittest.mock import patch, MagicMock
from pathlib import Path
import shutil
from dit_transfer.transfer import (
    parse_sftp_uri,
    parse_rclone_uri,
    file_checksum,
    transfer_local,
    is_ltfs_mount,
    spot_check_verify,
    directory_size_verify,
)


def test_parse_sftp_uri():
    uri = "sftp://user@example.com:2222/path/to/dir"
    user, host, port, path = parse_sftp_uri(uri)
    assert user == "user"
    assert host == "example.com"
    assert port == 2222
    assert path == "path/to/dir"

    uri2 = "sftp://example.com/path"
    user, host, port, path = parse_sftp_uri(uri2)
    assert host == "example.com"
    assert port == 22


def test_parse_rclone_uri():
    uri = "rclone://user:pass@remote:/path"
    remote, host, user, pw, port, path = parse_rclone_uri(uri)
    assert remote == "remote"
    assert user == "user"
    assert pw == "pass"
    assert port == 22
    assert path == "path"


def test_file_checksum(tmp_path):
    p = tmp_path / "test.bin"
    p.write_bytes(b"hello world\n")
    expected = hashlib.sha256(b"hello world\n").hexdigest()
    assert file_checksum(p) == expected


def test_transfer_local_file(tmp_path):
    src = tmp_path / "src.txt"
    src.write_text("content")
    dst = tmp_path / "dst.txt"
    transfer_local(src, dst)
    assert dst.read_text() == "content"
    assert dst.stat().st_size == src.stat().st_size


def test_transfer_local_dir(tmp_path):
    src = tmp_path / "src_dir"
    src.mkdir()
    (src / "f1.txt").write_text("one")
    sub = src / "sub"
    sub.mkdir()
    (sub / "f2.txt").write_text("two")
    dst = tmp_path / "dst_dir"
    transfer_local(src, dst)
    assert (dst / "f1.txt").read_text() == "one"
    assert (dst / "sub" / "f2.txt").read_text() == "two"


def test_transfer_local_verify(tmp_path):
    src = tmp_path / "verify.txt"
    src.write_text("verify content")
    dst = tmp_path / "v_dst.txt"
    transfer_local(src, dst, verify=True)
    # Should not raise


def test_transfer_rsync_fallback(tmp_path, monkeypatch):
    monkeypatch.setenv("PATH", os.environ.get("PATH", "") + ":")
    src = tmp_path / "rsync_src.txt"
    src.write_text("rsync")
    dst_dir = tmp_path / "dst_rsync"
    dst_dir.mkdir()
    with patch("subprocess.run") as mock_run:
        transfer_local(src, dst_dir, rsync_fallback=True)
        mock_run.assert_called()


def test_spot_check_verify(tmp_path):
    src_dir = tmp_path / "src_spot"
    src_dir.mkdir()
    large_file = src_dir / "large.bin"
    large_file.write_bytes(os.urandom(10 * 1024 * 1024))
    dst_dir = tmp_path / "dst_spot"
    dst_dir.mkdir()
    shutil.copy2(large_file, dst_dir / "large.bin")
    spot_check_verify(src_dir, dst_dir)


@patch("dit_transfer.transfer.subprocess")
def test_is_ltfs_mount_true(mock_subprocess, tmp_path):
    mock_result = MagicMock()
    mock_result.stdout = "ltfs filesystem\\n"
    mock_result.stderr = ""
    mock_result.returncode = 0
    mock_subprocess.run.return_value = mock_result
    assert is_ltfs_mount(tmp_path) is True


@patch("dit_transfer.transfer.subprocess")
def test_is_ltfs_mount_false(mock_subprocess, tmp_path):
    mock_result = MagicMock()
    mock_result.stdout = "ext4 filesystem\\n"
    mock_result.stderr = ""
    mock_result.returncode = 0
    mock_subprocess.run.return_value = mock_result
    assert is_ltfs_mount(tmp_path) is False


@patch("dit_transfer.transfer.subprocess")
def test_is_ltfs_mount_exception(mock_subprocess, tmp_path):
    mock_subprocess.run.side_effect = Exception("subprocess fail")
    assert is_ltfs_mount(tmp_path) is False


def test_is_ltfs_mount_volumes(tmp_path):
    # On Linux, /Volumes/LTO heuristic
    assert is_ltfs_mount(Path("/Volumes/LTO1")) is True


@patch("dit_transfer.transfer.subprocess")
def test_is_ltfs_mount_non_ltfs_volumes(mock_subprocess, tmp_path):
    # Mock to override subprocess, test non-LTFS path
    mock_result = MagicMock()
    mock_result.stdout = ""
    mock_result.returncode = 1
    mock_subprocess.run.return_value = mock_result
    assert is_ltfs_mount(Path("/not-volumes")) is False


def test_spot_check_verify_mismatch(tmp_path):
    src_dir = tmp_path / "src_spot_mismatch"
    dst_dir = tmp_path / "dst_spot_mismatch"
    src_dir.mkdir()
    dst_dir.mkdir()
    size = 2 * 1024 * 1024  # 2MB
    large_file = src_dir / "large.bin"
    large_file.write_bytes(b"a" * size)
    dst_file = dst_dir / "large.bin"
    dst_file.write_bytes(b"b" * size)
    with pytest.raises(RuntimeError, match="Spot-check chunk mismatch"):
        spot_check_verify(src_dir, dst_dir, num_chunks=1)


def test_directory_size_verify_missing(tmp_path):
    src_dir = tmp_path / "src_missing"
    dst_dir = tmp_path / "dst_missing"
    src_dir.mkdir()
    dst_dir.mkdir()
    (src_dir / "missing.txt").write_text("content")
    with pytest.raises(RuntimeError, match="File lists differ"):
        directory_size_verify(src_dir, dst_dir)


def test_directory_size_verify_size_mismatch(tmp_path):
    src_dir = tmp_path / "src_size"
    dst_dir = tmp_path / "dst_size"
    src_dir.mkdir()
    dst_dir.mkdir()
    f = src_dir / "mismatch.txt"
    f.write_text("abc")
    dst_f = dst_dir / "mismatch.txt"
    dst_f.write_text("ab")  # shorter
    with pytest.raises(RuntimeError, match="Size mismatches"):
        directory_size_verify(src_dir, dst_dir)
