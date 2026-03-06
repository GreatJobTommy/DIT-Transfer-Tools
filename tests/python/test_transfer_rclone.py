"""Tests for rclone transfer functions."""


from unittest.mock import patch, MagicMock, call, mock_open
from pathlib import Path
import configparser
import dit_transfer.transfer as dt


def test_parse_rclone_uri():
    result = dt.parse_rclone_uri("rclone://user:pass@remote:22/path")
    assert result == ("remote", "remote", "user", "pass", 22, "path")


@patch("dit_transfer.transfer.subprocess.check_output")
@patch("dit_transfer.transfer.Path.home")
@patch("builtins.open", new_callable=mock_open)
def test_ensure_rclone_remote(mock_file_open, mock_home, mock_check_output):
    mock_home.return_value = Path("/tmp")
    mock_check_output.return_value = b"obscuredpass\n"
    config = configparser.ConfigParser(interpolation=None)
    mock_file_open.return_value.__enter__.return_value.write = MagicMock()
    dt.ensure_rclone_remote("remote", "user", "pass", "host", 22)
    mock_check_output.assert_called_once_with(["rclone", "obscure", "pass"])


@patch("dit_transfer.transfer.subprocess.run")
def test_transfer_with_rclone(mock_run):
    dt.transfer_with_rclone("src", "dst", verify=True)
    mock_run.assert_has_calls(
        [
            call(
                [
                    "rclone",
                    "copy",
                    "src",
                    "dst",
                    "--progress",
                    "--checksum",
                    "--transfers",
                    "4",
                ],
                check=True,
            ),
            call(["rclone", "check", "src", "dst", "--download"], check=True),
        ],
        any_order=True,
    )


@patch("dit_transfer.transfer.subprocess.check_output")
@patch("dit_transfer.transfer.parse_sftp_uri")
def test_create_temp_rclone_sftp_remote(mock_parse, mock_check_output):
    mock_parse.return_value = ("user", "host", 22, "path")
    result = dt.create_temp_rclone_sftp_remote("sftp://user@host/path", "pass")
    assert result[0].startswith("temp_sftp_")


@patch("dit_transfer.transfer.Path.home")
@patch("builtins.open", new_callable=mock_open)
def test_cleanup_temp_rclone_remote(mock_file_open, mock_home):
    mock_home.return_value = Path("/tmp")
    conf_path = Path.home() / ".config" / "rclone" / "rclone.conf"
    config = configparser.ConfigParser(interpolation=None)
    config["testremote"] = {"type": "sftp"}
    mock_write = MagicMock()
    mock_file_open.return_value.__enter__.return_value.write = mock_write
    dt.cleanup_temp_rclone_remote("testremote")
    assert mock_file_open.call_count == 0
