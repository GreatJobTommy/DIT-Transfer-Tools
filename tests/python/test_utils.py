from .utils import get_dir_size


def test_get_dir_size(tmp_path):
    d = tmp_path / "d"
    d.mkdir()
    (d / "f1").write_bytes(b"a")
    sub = d / "sub"
    sub.mkdir()
    (sub / "f2").write_bytes(b"bb")
    assert get_dir_size(d) == 1 + 2
