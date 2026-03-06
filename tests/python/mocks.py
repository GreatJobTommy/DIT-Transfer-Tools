class MockPath:
    def __init__(self, is_dir=False, size=0):
        self.is_dir = is_dir
        self._size = size

    def stat(self):
        class Stat:
            st_size = self._size

        return Stat()

    @property
    def is_file(self):
        return not self.is_dir

    @property
    def is_dir(self):
        return self.is_dir

    def exists(self):
        return True

    def rglob(self, pat):
        if not self.is_dir:
            return []
        return [MockPath(is_dir=True)]

    def relative_to(self, other):
        return self

    name = "testfile.txt"
