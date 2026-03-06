import os


def get_dir_size(path):
    total = 0
    for root, dirs, files in os.walk(path):
        for file in files:
            total += os.path.getsize(os.path.join(root, file))
    return total
