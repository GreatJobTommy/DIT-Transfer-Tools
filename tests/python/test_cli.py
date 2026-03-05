from click.testing import CliRunner
from dit_transfer.cli import cli
import os
import pytest
from unittest.mock import patch, MagicMock
import stat


def test_cli_help():
    runner = CliRunner()
    result = runner.invoke(cli, ["--help"])
    assert result.exit_code == 0
    assert "Usage" in result.stdout
    assert "transfer" in result.stdout
    assert "verify" in result.stdout


def test_transfer_local_file(tmp_path):
    runner = CliRunner()
    src = tmp_path / "test.txt"
    src.write_text("hello cli")
    dst_dir = tmp_path / "dst"
    dst_dir.mkdir()
    result = runner.invoke(cli, ["transfer", str(src), str(dst_dir)])
    assert result.exit_code == 0
    dst_file = dst_dir / "test.txt"
    assert dst_file.exists()
    assert dst_file.read_text() == "hello cli"


def test_transfer_local_dir(tmp_path):
    runner = CliRunner()
    src_dir = tmp_path / "src_dir"
    src_dir.mkdir()
    (src_dir / "file.txt").write_text("dir content")
    dst_dir = tmp_path / "dst_dir"
    result = runner.invoke(cli, ["transfer", str(src_dir), str(dst_dir)])
    assert result.exit_code == 0
    assert (dst_dir / "file.txt").read_text() == "dir content"


def test_transfer_verify(tmp_path):
    runner = CliRunner()
    src = tmp_path / "verify.txt"
    src.write_text("verify")
    dst_dir = tmp_path / "dst_v"
    dst_dir.mkdir()
    result = runner.invoke(cli, ["transfer", str(src), str(dst_dir), "--verify"])
    assert result.exit_code == 0


def test_transfer_rsync_fallback(tmp_path):
    runner = CliRunner()
    src = tmp_path / "rsync.txt"
    src.write_text("rsync test")
    dst_dir = tmp_path / "dst_rsync"
    dst_dir.mkdir()
    result = runner.invoke(
        cli, ["transfer", str(src), str(dst_dir), "--rsync-fallback"]
    )
    assert result.exit_code == 0
    dst_file = dst_dir / "rsync.txt"
    assert dst_file.exists()


def test_transfer_spot_check(tmp_path):
    runner = CliRunner()
    src = tmp_path / "spot.txt"
    src.write_bytes(os.urandom(10 * 1024 * 1024))  # 10MB for chunks
    dst_dir = tmp_path / "dst_spot"
    dst_dir.mkdir()
    result = runner.invoke(
        cli, ["transfer", str(src), str(dst_dir), "--spot-check-hashes"]
    )
    assert result.exit_code == 0
    dst_file = dst_dir / "spot.txt"
    assert dst_file.exists()


def test_transfer_concurrency(tmp_path):
    runner = CliRunner()
    src = tmp_path / "conc.txt"
    src.write_text("concurrency test")
    dst_dir = tmp_path / "dst_c"
    dst_dir.mkdir()
    result = runner.invoke(
        cli, ["transfer", str(src), str(dst_dir), "--concurrency", "2"]
    )
    assert result.exit_code == 0


def test_verify_match(tmp_path):
    runner = CliRunner()
    src_dir = tmp_path / "src"
    src_dir.mkdir()
    (src_dir / "test.txt").write_text("content")
    dst_dir = tmp_path / "dst"
    dst_dir.mkdir()
    (dst_dir / "test.txt").write_text("content")
    result = runner.invoke(cli, ["verify", str(src_dir), str(dst_dir)])
    assert result.exit_code == 0
    assert "All files match by size" in result.stdout


def test_verify_missing(tmp_path):
    runner = CliRunner()
    src_dir = tmp_path / "src_m"
    src_dir.mkdir()
    (src_dir / "miss.txt").write_text("miss")
    dst_dir = tmp_path / "dst_m"
    dst_dir.mkdir()
    result = runner.invoke(cli, ["verify", str(src_dir), str(dst_dir)])
    assert result.exit_code == 1
    assert "Missing files in DEST" in result.stdout


def test_verify_mismatch(tmp_path):
    runner = CliRunner()
    src_dir = tmp_path / "src_mm"
    src_dir.mkdir()
    (src_dir / "mm.txt").write_text("short")  # len 5
    dst_dir = tmp_path / "dst_mm"
    dst_dir.mkdir()
    (dst_dir / "mm.txt").write_text("longer")  # len 6
    result = runner.invoke(cli, ["verify", str(src_dir), str(dst_dir)])
    assert result.exit_code == 1
    assert "Size mismatches" in result.stdout


def test_verify_not_dir(tmp_path):
    runner = CliRunner()
    src_file = tmp_path / "notdir.txt"
    src_file.write_text("file")
    dst_dir = tmp_path / "dst_nd"
    dst_dir.mkdir()
    result = runner.invoke(cli, ["verify", str(src_file), str(dst_dir)])
    assert result.exit_code == 1
    assert "Error" in result.output


@patch("dit_transfer.cli.sftp_connect")
def test_transfer_local_to_sftp(mock_connect, tmp_path):
    mock_sftp = MagicMock()
    mock_client = MagicMock()
    mock_connect.return_value = (mock_sftp, mock_client)
    runner = CliRunner()
    src = tmp_path / "file.txt"
    src.write_text("test")
    result = runner.invoke(
        cli, ["transfer", str(src), "sftp://user@host:/remote", "--password", "pass"]
    )
    assert result.exit_code == 0
    mock_connect.assert_called_once()


@patch("dit_transfer.cli.sftp_connect")
def test_transfer_sftp_to_local(mock_connect, tmp_path):
    mock_sftp = MagicMock()
    mock_client = MagicMock()
    mock_connect.return_value = (mock_sftp, mock_client)
    mock_st = MagicMock()
    mock_st.st_mode = stat.S_IFDIR
    mock_sftp.stat.return_value = mock_st
    mock_sftp.listdir_attr.return_value = []
    runner = CliRunner()
    dst = tmp_path / "dir"
    result = runner.invoke(
        cli, ["transfer", "sftp://user@host:/remote", str(dst), "--password", "pass"]
    )
    assert result.exit_code == 0
    mock_connect.assert_called_once()


@patch("dit_transfer.cli.transfer_with_rclone")
def test_transfer_rclone(mock_rclone, tmp_path):
    runner = CliRunner()
    src = tmp_path / "file.txt"
    src.write_text("test")
    result = runner.invoke(cli, ["transfer", str(src), "rclone://remote:/path"])
    assert result.exit_code == 0
    mock_rclone.assert_called_once()


@patch("dit_transfer.cli.ensure_rclone_remote")
@patch("dit_transfer.cli.transfer_with_rclone")
def test_transfer_sftp_rclone(mock_rclone, mock_ensure, tmp_path):
    runner = CliRunner()
    result = runner.invoke(
        cli,
        [
            "transfer",
            "sftp://user@host:/remote",
            "rclone://remote2:/path",
            "--password",
            "pass",
        ],
    )
    assert result.exit_code == 0
    mock_rclone.assert_called_once()
    assert "temp_sftp" in str(mock_rclone.call_args[0][0])


def test_transfer_invalid_sftp_uri(tmp_path):
    runner = CliRunner()
    dst_dir = tmp_path / "dst"
    dst_dir.mkdir()
    result = runner.invoke(
        cli, ["transfer", "sftp:///remote", str(dst_dir), "--password", "dummy"]
    )
    assert result.exit_code != 0
    assert "Error" in result.output


def test_transfer_invalid_rclone_uri(tmp_path):
    runner = CliRunner()
    src_file = tmp_path / "test.txt"
    src_file.write_text("test")
    result = runner.invoke(
        cli,
        [
            "transfer",
            str(src_file),
            "rclone:///bad",
        ],
    )
    assert result.exit_code != 0
    assert "Error" in result.output


def test_transfer_sftp_sftp_notimpl(tmp_path):
    runner = CliRunner()
    result = runner.invoke(
        cli,
        [
            "transfer",
            "sftp://user@host:/a",
            "sftp://user2@host:/b",
            "--password",
            "pass",
        ],
    )
    assert result.exit_code != 0
    assert "Direct SFTP to SFTP transfer not supported yet." in result.output


@patch("dit_transfer.cli.sftp_connect")
@patch("click.prompt")
def test_transfer_sftp_prompt(mock_prompt, mock_connect, tmp_path):
    mock_prompt.return_value = "secret"
    mock_sftp = MagicMock()
    mock_client = MagicMock()
    mock_connect.return_value = (mock_sftp, mock_client)
    mock_st = MagicMock()
    mock_st.st_mode = stat.S_IFDIR
    mock_sftp.stat.return_value = mock_st
    mock_sftp.listdir_attr.return_value = []
    runner = CliRunner()
    dst_dir = tmp_path / "dst"
    dst_dir.mkdir()
    result = runner.invoke(cli, ["transfer", "sftp://user@host:/remote", str(dst_dir)])
    assert result.exit_code == 0
    mock_prompt.assert_called_once_with("Enter SFTP password", hide_input=True)


@patch("dit_transfer.cli.sftp_connect")
@patch("click.prompt")
def test_transfer_key_file(mock_prompt, mock_connect, tmp_path):
    mock_prompt.return_value = "pass"
    mock_sftp = MagicMock()
    mock_client = MagicMock()
    mock_connect.return_value = (mock_sftp, mock_client)
    mock_st_dir = MagicMock()
    mock_st_dir.st_mode = stat.S_IFDIR
    mock_sftp.stat.return_value = mock_st_dir
    runner = CliRunner()
    src_file = tmp_path / "test.txt"
    src_file.write_text("test")
    result = runner.invoke(
        cli,
        [
            "transfer",
            str(src_file),
            "sftp://user@host:/remote",
            "--key-file",
            "/path/to/key",
        ],
    )
    assert result.exit_code == 0
    args = mock_connect.call_args[0]
    assert args[4] == "/path/to/key"
    mock_prompt.assert_called_once()


@patch("dit_transfer.cli.transfer_with_rclone")
@patch("dit_transfer.cli.ensure_rclone_remote")
def test_transfer_rclone_dest_auth(mock_ensure, mock_rclone, tmp_path):
    runner = CliRunner()
    src_file = tmp_path / "test.txt"
    src_file.write_text("test")
    result = runner.invoke(
        cli, ["transfer", str(src_file), "rclone://user:pass@remote:/path"]
    )
    assert result.exit_code == 0
    mock_ensure.assert_called_once()


@patch("dit_transfer.cli.transfer_with_rclone")
@patch("dit_transfer.cli.ensure_rclone_remote")
def test_transfer_rclone_source_auth(mock_ensure, mock_rclone, tmp_path):
    runner = CliRunner()
    dst_dir = tmp_path / "dst"
    dst_dir.mkdir()
    result = runner.invoke(
        cli, ["transfer", "rclone://user:pass@remote:/path", str(dst_dir)]
    )
    assert result.exit_code == 0
    mock_ensure.assert_called_once()
