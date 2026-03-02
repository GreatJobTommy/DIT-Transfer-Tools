import pytest
from click.testing import CliRunner
from pathlib import Path
from dit_transfer.cli import main

def test_cli_help():
    runner = CliRunner()
    result = runner.invoke(main, ['--help'])
    assert result.exit_code == 0
    assert 'Usage' in result.stdout

def test_cli_local_file(tmp_path):
    runner = CliRunner()
    src = tmp_path / 'test.txt'
    src.write_text('hello cli')
    dst_dir = tmp_path / 'dst'
    result = runner.invoke(main, [str(src), str(dst_dir)])
    assert result.exit_code == 0
    dst_file = dst_dir / 'test.txt'
    assert dst_file.exists()
    assert dst_file.read_text() == 'hello cli'

def test_cli_local_dir(tmp_path):
    runner = CliRunner()
    src_dir = tmp_path / 'src_dir'
    src_dir.mkdir()
    (src_dir / 'file.txt').write_text('dir content')
    dst_dir = tmp_path / 'dst_dir'
    result = runner.invoke(main, [str(src_dir), str(dst_dir)])
    assert result.exit_code == 0
    assert (dst_dir / 'file.txt').read_text() == 'dir content'

def test_cli_verify(tmp_path):
    runner = CliRunner()
    src = tmp_path / 'verify.txt'
    src.write_text('verify')
    dst_dir = tmp_path / 'dst_v'
    result = runner.invoke(main, [str(src), str(dst_dir), '--verify'])
    assert result.exit_code == 0

def test_cli_concurrency(tmp_path):
    runner = CliRunner()
    src = tmp_path / 'conc.txt'
    src.write_text('concurrency test')
    dst_dir = tmp_path / 'dst_c'
    result = runner.invoke(main, [str(src), str(dst_dir), '--concurrency', '2'])
    assert result.exit_code == 0