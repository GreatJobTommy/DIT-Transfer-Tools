import pytest
from click.testing import CliRunner
from pathlib import Path
from dit_transfer.cli import cli

def test_cli_help():
    runner = CliRunner()
    result = runner.invoke(cli, ['--help'])
    assert result.exit_code == 0
    assert 'Usage' in result.stdout
    assert 'transfer' in result.stdout
    assert 'verify' in result.stdout

def test_transfer_local_file(tmp_path):
    runner = CliRunner()
    src = tmp_path / 'test.txt'
    src.write_text('hello cli')
    dst_dir = tmp_path / 'dst'
    dst_dir.mkdir()
    result = runner.invoke(cli, ['transfer', str(src), str(dst_dir)])
    assert result.exit_code == 0
    dst_file = dst_dir / 'test.txt'
    assert dst_file.exists()
    assert dst_file.read_text() == 'hello cli'

def test_transfer_local_dir(tmp_path):
    runner = CliRunner()
    src_dir = tmp_path / 'src_dir'
    src_dir.mkdir()
    (src_dir / 'file.txt').write_text('dir content')
    dst_dir = tmp_path / 'dst_dir'
    result = runner.invoke(cli, ['transfer', str(src_dir), str(dst_dir)])
    assert result.exit_code == 0
    assert (dst_dir / 'file.txt').read_text() == 'dir content'

def test_transfer_verify(tmp_path):
    runner = CliRunner()
    src = tmp_path / 'verify.txt'
    src.write_text('verify')
    dst_dir = tmp_path / 'dst_v'
    dst_dir.mkdir()
    result = runner.invoke(cli, ['transfer', str(src), str(dst_dir), '--verify'])
    assert result.exit_code == 0

def test_transfer_concurrency(tmp_path):
    runner = CliRunner()
    src = tmp_path / 'conc.txt'
    src.write_text('concurrency test')
    dst_dir = tmp_path / 'dst_c'
    dst_dir.mkdir()
    result = runner.invoke(cli, ['transfer', str(src), str(dst_dir), '--concurrency', '2'])
    assert result.exit_code == 0

def test_verify_match(tmp_path):
    runner = CliRunner()
    src_dir = tmp_path / 'src'
    src_dir.mkdir()
    (src_dir / 'test.txt').write_text('content')
    dst_dir = tmp_path / 'dst'
    dst_dir.mkdir()
    (dst_dir / 'test.txt').write_text('content')
    result = runner.invoke(cli, ['verify', str(src_dir), str(dst_dir)])
    assert result.exit_code == 0
    assert 'All files match by size' in result.stdout

def test_verify_missing(tmp_path):
    runner = CliRunner()
    src_dir = tmp_path / 'src_m'
    src_dir.mkdir()
    (src_dir / 'miss.txt').write_text('miss')
    dst_dir = tmp_path / 'dst_m'
    dst_dir.mkdir()
    result = runner.invoke(cli, ['verify', str(src_dir), str(dst_dir)])
    assert result.exit_code == 1
    assert 'Missing files in DEST' in result.stdout

def test_verify_mismatch(tmp_path):
    runner = CliRunner()
    src_dir = tmp_path / 'src_mm'
    src_dir.mkdir()
    (src_dir / 'mm.txt').write_text('short')  # len 5
    dst_dir = tmp_path / 'dst_mm'
    dst_dir.mkdir()
    (dst_dir / 'mm.txt').write_text('longer')  # len 6
    result = runner.invoke(cli, ['verify', str(src_dir), str(dst_dir)])
    assert result.exit_code == 1
    assert 'Size mismatches' in result.stdout

def test_verify_not_dir(tmp_path):
    runner = CliRunner()
    src_file = tmp_path / 'notdir.txt'
    src_file.write_text('file')
    dst_dir = tmp_path / 'dst_nd'
    dst_dir.mkdir()
    result = runner.invoke(cli, ['verify', str(src_file), str(dst_dir)])
    assert result.exit_code == 1
    assert 'Error' in result.output