import pytest
import os

import configparser
from unittest.mock import Mock, patch, MagicMock
from pathlib import Path
from dit_transfer.transfer import (
    get_dir_size_raise,
    sftp_connect,
    ensure_rclone_remote,
    cleanup_temp_rclone_remote,
    spot_check_verify,
)


def test_get_dir_size_raise():
    with pytest.raises(ValueError, match="test"):
        get_dir_size_raise(Path("/"))


def test_spot_check_multiple_large(tmp_path):
    src_dir = tmp_path / "src"
    src_dir.mkdir()
    chunk_size = 1024 * 1024  # 1MB
    num_large = 10
    for i in range(num_large):
        f = src_dir / f"large{i}.bin"
        data = os.urandom(chunk_size + 1)
        f.write_bytes(data)
    dst_dir = tmp_path / "dst"
    dst_dir.mkdir()
    for f in src_dir.rglob("*"):
        dst_f = dst_dir / f.relative_to(src_dir)
        dst_f.write_bytes(f.read_bytes())
    spot_check_verify(src_dir, dst_dir, num_chunks=5, chunk_mb=1)


@patch("paramiko.RSAKey.from_private_key_file")
@patch("os.path.exists")
@patch("paramiko.SSHClient")
def test_sftp_connect_key_file(mock_ssh, mock_exists, mock_rsa):
    mock_exists.return_value = True
    mock_pkey = Mock()
    mock_rsa.return_value = mock_pkey
    mock_client = MagicMock()
    mock_sftp = MagicMock()
    mock_client.open_sftp.return_value = mock_sftp
    mock_ssh.return_value = mock_client
    sftp, client = sftp_connect("user", "host", 22, key_file="/fakekey")
    mock_client.connect.assert_called_once_with(
        hostname="host", port=22, username="user", pkey=mock_pkey
    )
    mock_client.open_sftp.assert_called_once()


@patch("pathlib.Path.home")
@patch("subprocess.check_output")
def test_ensure_rclone_remote_obscure(mock_check_output, mock_home):
    mock_home.return_value = Path("/tmp/nonexistent")
    mock_check_output.return_value = b"***Obf***\n"
    ensure_rclone_remote("testrem", "u", "pass", "h", 22)
    mock_check_output.assert_called_once_with(["rclone", "obscure", "pass"])


def test_cleanup_temp_rclone_remote_delete(tmp_path):
    with patch("pathlib.Path.home", return_value=tmp_path):
        conf_path = tmp_path / ".config" / "rclone" / "rclone.conf"
        conf_path.parent.mkdir(parents=True, exist_ok=True)
        conf_content = """
[testrem]
type = sftp
host = h
"""
        conf_path.write_text(conf_content)
        cleanup_temp_rclone_remote("testrem")
        config = configparser.ConfigParser(interpolation=None)
        if conf_path.exists():
            config.read(conf_path)
        assert "testrem" not in config
