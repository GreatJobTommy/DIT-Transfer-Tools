import dit_transfer.version

def test_version():
    from dit_transfer.version import __version__
    assert __version__ == "4.4.0-dev"
