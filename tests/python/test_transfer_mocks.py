import pytest
from unittest.mock import Mock, patch, MagicMock
import paramiko
from pathlib import Path
from dit_transfer.transfer import (
    parse_sftp_uri, sftp_connect, transfer_local_to_sftp, transfer_sftp_to_local,
    transfer_local, parse_rclone_uri, ensure_rclone_remote, transfer_with_rclone
)

def test_parse_sftp_uri():
    user, host, port, path = parse_sftp_uri('sftp://user:pass@host:22/path')
    assert user == 'user'
    assert host == 'host'
    assert port == 22
    assert path == 'path'

def test_parse_rclone_uri():
    remote, host, user, pw, port, path = parse_rclone_uri('rclone://user:pass@remote:22/path')
    assert remote == 'remote'
    assert user == 'user'
    assert pw == 'pass'

@patch('subprocess.check_output')
def test_ensure_rclone_remote(mock_subprocess):
    mock_subprocess.return_value = b'obfpass\n'
    ensure_rclone_remote('testremote', 'user', 'pass', 'host', 22)

@patch('paramiko.SSHClient')
def test_sftp_connect(mock_ssh):
    mock_client = MagicMock()
    mock_sftp = MagicMock()
    mock_client.open_sftp.return_value = mock_sftp
    mock_ssh.return_value = mock_client
    sftp, client = sftp_connect('user', 'host', 22, password='pass')
    mock_client.connect.assert_called()

@patch('dit_transfer.transfer.sftp_makedirs')
@patch('dit_transfer.transfer.ProgressTracker')
@patch('tqdm.tqdm')
def test_transfer_local_to_sftp_file(mock_tqdm, mock_tracker, mock_makedirs):
    mock_sftp = MagicMock()
    mock_path = MagicMock()
    mock_path.is_dir.return_value = False
    mock_path.stat.return_value = Mock(st_size=100)
    transfer_local_to_sftp(mock_path, '/remote', mock_sftp)

# Similar for other functions, stubs for coverage
@patch('dit_transfer.transfer.directory_size_verify')
@patch('dit_transfer.transfer.spot_check_verify')


@patch('dit_transfer.transfer.subprocess.run')
def test_transfer_with_rclone(mock_run, mock_spot_check_verify, mock_directory_size_verify):
    transfer_with_rclone('src:', 'dst:')
    mock_run.assert_called()
