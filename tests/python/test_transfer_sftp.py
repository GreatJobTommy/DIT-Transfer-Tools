"""Tests for SFTP transfer functions."""
import pytest
from unittest.mock import patch, MagicMock, call
from pathlib import Path
import dit_transfer.transfer as dt

class TestSFTP:
    @patch('dit_transfer.transfer.os.path.exists')
    @patch('dit_transfer.transfer.paramiko')
    def test_sftp_connect_key(self, mock_paramiko, mock_exists):
        mock_exists.return_value = True
        mock_client = MagicMock()
        mock_paramiko.SSHClient.return_value = mock_client
        mock_paramiko.AutoAddPolicy.return_value = MagicMock()
        mock_paramiko.RSAKey.from_private_key_file.return_value = MagicMock()
        dt.sftp_connect('user', 'host', 22, key_file='/key')
        mock_client.connect.assert_called_once_with(hostname='host', port=22, username='user', pkey=mock_paramiko.RSAKey.from_private_key_file.return_value)
        mock_client.open_sftp.assert_called_once()

    @patch('dit_transfer.transfer.paramiko')
    def test_sftp_connect_password(self, mock_paramiko):
        mock_client = MagicMock()
        mock_paramiko.SSHClient.return_value = mock_client
        mock_paramiko.AutoAddPolicy.return_value = MagicMock()
        dt.sftp_connect('user', 'host', 22, password='pass')
        mock_client.connect.assert_called_once_with(hostname='host', port=22, username='user', password='pass')
        mock_client.open_sftp.assert_called_once()


    def test_sftp_makedirs_not_exists(self):
        mock_sftp = MagicMock()
        mock_sftp.stat.side_effect = IOError
        dt.sftp_makedirs(mock_sftp, '/remote/dir')
        mock_sftp.mkdir.assert_has_calls([call('/remote'), call('/remote/dir')])


    def test_sftp_makedirs_exists(self):
        mock_sftp = MagicMock()
        mock_sftp.stat.return_value = MagicMock()
        dt.sftp_makedirs(mock_sftp, '/remote/dir')
        mock_sftp.mkdir.assert_not_called()


    def test_sftp_makedirs_recursive(self):
        mock_sftp = MagicMock()
        mock_sftp.stat.side_effect = [IOError] * 3
        dt.sftp_makedirs(mock_sftp, '/remote/a/b')
        mock_sftp.mkdir.assert_has_calls([call('/remote/a'), call('/remote/a/b')])

    @patch('dit_transfer.transfer.hashlib')
    def test_remote_file_checksum(self, mock_hashlib):
        mock_h = MagicMock()
        mock_h.hexdigest.return_value = 'hash'
        mock_hashlib.new.return_value = mock_h
        mock_sftp = MagicMock()
        mock_file = MagicMock()
        mock_sftp.open.return_value.__enter__.return_value = mock_file
        mock_file.read.side_effect = [b'data', b'']
        result = dt.remote_file_checksum(mock_sftp, '/remote/file')
        assert result == 'hash'

    @patch('dit_transfer.transfer.remote_file_checksum')
    @patch('dit_transfer.transfer.file_checksum')
    @patch('dit_transfer.transfer.tqdm')
    def test_transfer_local_to_sftp_file_verify(self, mock_tqdm, mock_file_cs, mock_remote_cs):
        mock_pbar = MagicMock()
        mock_tqdm.return_value.__enter__.return_value = mock_pbar
        mock_file_cs.return_value = 'hash'
        mock_remote_cs.return_value = 'hash'
        mock_sftp = MagicMock()
        local_path = MagicMock(spec=Path)
        local_path.is_dir.return_value = False
        local_path.name = 'file'
        local_path.stat.return_value.st_size = 100
        dt.transfer_local_to_sftp(local_path, '/remote', mock_sftp, verify=True)
        mock_sftp.put.assert_called_once()

    @patch('dit_transfer.transfer.tqdm')
    @patch("os.scandir")
    def test_transfer_local_to_sftp_dir(self, mock_tqdm):
        mock_pbar = MagicMock()
        mock_tqdm.return_value.__enter__.return_value = mock_pbar
        mock_sftp = MagicMock()
        local_path = MagicMock(spec=Path)
        local_path.is_dir.return_value = True
        mock_file1 = MagicMock(spec=Path)
        mock_file1.is_file.return_value = True
        mock_file1.stat.return_value.st_size = 100
        mock_file2 = MagicMock(spec=Path)
        mock_file2.is_file.return_value = True
        mock_file2.stat.return_value.st_size = 200
        mock_scandir.return_value = iter([mock_file1, mock_file2])
        dt.transfer_local_to_sftp(local_path, '/remote', mock_sftp)
        mock_tqdm.assert_called_once_with(total=300, unit='B', unit_scale=True, desc='Uploading')

    @patch('dit_transfer.transfer.tqdm')
    
    
    @patch("pathlib.Path.is_dir")
    def test_transfer_sftp_to_local_dir(self, mock_tqdm, mock_is_dir):
        mock_is_dir.return_value = True
        mock_st = MagicMock()
        mock_sftp.stat.return_value = mock_st
        mock_pbar = MagicMock()
        mock_tqdm.return_value.__enter__.return_value = mock_pbar
        mock_sftp = MagicMock()
        dt.transfer_sftp_to_local('/remote', Path('/local'), mock_sftp)
        mock_tqdm.assert_called_once()

    @pytest.mark.parametrize('transferred,total,expected_update', [(50,100,50), (100,200,100)])
    def test_progress_tracker_callback(self, transferred, total, expected_update):
        pbar = MagicMock()
        tracker = dt.ProgressTracker(pbar)
        tracker.callback(transferred, total)
        pbar.update.assert_called_once_with(expected_update)
