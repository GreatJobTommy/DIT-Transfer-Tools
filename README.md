# DIT-Transfer-Tools

Suite of command-line tools for efficient, secure, and reliable data transfers optimized for DIT (Digital Imaging Technician) workflows on film sets and post-production.

## Features
- High-speed file transfer with progress tracking (shutil + tqdm, rclone)
- Checksum verification after transfer
- SFTP support (paramiko)
- Rclone remotes support
- Verify mode (file size comparison)
- pytest tests

## Installation
```bash
pip install dit-transfer-tools
```

## Usage
```bash
dit-transfer --help
```

### Transfer
```bash
dit-transfer transfer SOURCE DEST [--verify] [--password PASSWORD] [--key-file KEY] [--concurrency N]
```
SOURCE/DEST:
- local path
- sftp://user@host:port/path
- rclone://remote:path

Example:
```bash
dit-transfer transfer /local/dir sftp://user@server:/remote/dir --verify
```

### Verify (v1: size check only, local/local)
```bash
dit-transfer verify /src/dir /dest/dir [--dry-run]
```
Reports:
- Missing files in DEST
- Extra files in DEST
- Size mismatches

Exit 0 if all match, 1 otherwise.

## Roadmap
1. Core CLI ✓
2. Verify mode ✓
3. LTO integration
4. GUI wrapper

Built with Python 3.12+, Click, paramiko, tqdm, cryptography.