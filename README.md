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
1-8. Complete ✓

Packaging & releases complete up to v4.1.0.

## Changelog

**2026-03-03 - GUI Polish (roadmap #5)**  
- Ctrl+N: add task  
- Del: remove selected queue item  
- Drag-drop files to waiting queue  
- Theme toggle saves/loads  
- Button tooltips  
- Task failure QMessageBox  

**2026-03-06 - v4.7.0**  
- feat(v4.7.0): Full tape-aware verification + rsync whole-file fallback [PR #57](https://github.com/GreatJobTommy/DIT-Transfer-Tools/pull/57)  
- Tape verify: size check all files + 5x random 1MB SHA256 spot-checks (large files)  
- Rsync LTFS dest fallback: --whole-file -a --info=progress2  
- GUI presets/LTO options + verify checkbox  
- Tests/build verified  
Closes [#56](https://github.com/GreatJobTommy/DIT-Transfer-Tools/issues/56)

**2026-03-06 - v4.6.0**  
- feat(v4.6): LTFS rsync fallback + GUI presets + tape verify stub (PR #55)  
- RSYNC fallback for LTFS metadata/xattrs  
- GUI LTO presets (buffer, sequential, rsync checkbox)  
- Tape spot-check verify (size + random chunks)  
- rclone:// URI support + auto-remote creation  
- pytest 85% cov  

## LTO/LTFS Tape Support 🎞️

DIT-Transfer-Tools now provides optimized support for Linear Tape File System (LTFS) on LTO tapes:

- **Auto-detection**: Identifies LTFS mounts via `QStorageInfo::fileSystemType() == &quot;ltfs&quot;` (works on macOS `/Volumes/LTO*`, Linux `/mnt/ltfs`, etc.).
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