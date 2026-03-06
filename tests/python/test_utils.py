def test_get_dir_size(tmp_path):
    d = tmp_path / &quot;d&quot;
    d.mkdir()
    (d / &quot;f1&quot;).write_bytes(b&quot;a&quot;)
    sub = d / &quot;sub&quot;
    sub.mkdir()
    (sub / &quot;f2&quot;).write_bytes(b&quot;bb&quot;)
    assert get_dir_size(d) == 1 + 2