import pytest

from dit_transfer.transfer import spot_check_verify


def test_spot_check_verify_missing(tmp_path):
    src_dir = tmp_path / "src"
    src_dir.mkdir()
    (src_dir / "file.txt").write_text("data")
    dst_dir = tmp_path / "dst"
    dst_dir.mkdir()
    # dst missing the file
    with pytest.raises((ValueError, AssertionError)):
        spot_check_verify(src_dir, dst_dir, num_chunks=1, chunk_mb=1)
