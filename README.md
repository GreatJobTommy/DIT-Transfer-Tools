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
3. LTO integration ✓
4. GUI LTO presets & tests
5. GUI wrapper polish

## LTO/LTFS Tape Support 🎞️

DIT-Transfer-Tools now provides optimized support for Linear Tape File System (LTFS) on LTO tapes:

- **Auto-detection**: Identifies LTFS mounts via `QStorageInfo::fileSystemType() == "ltfs"` (works on macOS `/Volumes/LTO*`, Linux `/mnt/ltfs`, etc.).
- **Sequential buffer**: Automatically sets 64MB read/write buffer for high-performance sequential tape writes, reducing head movement.
- **Progress monitoring**: `onReadyRead()` stub ready for parsing rsync/rclone output.
- **Planned**:
  - Rsync `--whole-file -a` fallback for full metadata/extended attributes preservation.
  - Tape-aware verification: size match + spot-check hashes (e.g., 5x 1MB random chunks).
  - GUI: LTO presets in Add Task dialog, drive presets.

**Examples**:
```
Destination: /Volumes/LTO6/Projects/2026-Film/
- Auto LTO mode ✓
- Optimized writes ✓
```

Mount LTFS and transfer – seamless!

Built with Python 3.12+, Click, paramiko, tqdm, cryptography.

## LTO Support

LTFS mount points fully supported as local paths.

- Progress bars for slow random access
- Copies file data/metadata (no hard links)
- Verify by size (local) or hash compatible

Example:
```
dit-transfer transfer ~/data /Volumes/LTO6/backup --verify
```

Closes #32: feat LTO integration.