# CHANGELOG

## v4.4.0 (2026-03-05)
### Added
- Rclone Remotes tab in MainWindow: QListWidget remotes, refresh/add/edit/delete buttons using RcloneRemoteDialog (SFTP focus, obscure pass)
- Full integration: AddTaskDialog parses rclone://remote:path, shows rclone group, refresh remotes from ~/.config/rclone/rclone.conf
- TransferTask: isRcloneRemote(), rclone copy --progress --checksum --transfers=N with presets (S3 Fast, GDrive, Safe Copy)
- RcloneRemoteDialog: GUI for SFTP remote create/edit (name/host/user/port/pass→obscure→rclone config create)
- C++ QtTest in tests/test_rclone.pro for dialog/tab (mock QProcess)

### Fixed
- CLI transfer logic: restructure if/elif chains to prevent double rclone calls in mixed sftp+rclone paths
- Test mocks: stat.st_mode = stat.S_IFDIR (16384) for dir detection
- Tests: add coverage for parse_uri exceptions, click.prompt paths, key_file auth, verify errors (61/61 pass, 85% cov)

### Tests
- pytest-cov >=85%, omit gui/__init__.py parse errors

## v4.2.1 (2026-03-05)
### Fixed
- PyPI version bump for patch release readiness
- Reference upcoming CI release fixes (#49)

## v4.2.0 (2026-03-05)
### Added
- LTFS enhancements (#47)

### Fixed
- CI: correct bash syntax in build.yml and release.yml (#48)

## v4.1.0 (2026-03-04)
### Added
- Basic tkinter GUI prototype (roadmap #4-5) (#45)
- rclone:// backend support for LTO/rclone integration per README roadmap #1
- Parse rclone://[user:pass@]remote:path URIs
- Auto-create SFTP rclone remotes from URI credentials using rclone obscure
- rclone copy with --progress --checksum --transfers=N [--concurrency 4]
- --verify uses rclone check --download
- Support local ↔ rclone, rclone ↔ rclone (no paramiko SFTP mix yet)

[... rest unchanged]