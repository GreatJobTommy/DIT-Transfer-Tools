# CHANGELOG

## Unreleased
### Added
- rclone:// backend support for LTO/rclone integration per README roadmap #1
- Parse rclone://[user:pass@]remote:path URIs
- Auto-create SFTP rclone remotes from URI credentials using rclone obscure
- rclone copy with --progress --checksum --transfers=N [--concurrency 4]
- --verify uses rclone check --download
- Support local ↔ rclone, rclone ↔ rclone (no paramiko SFTP mix yet)

## v4.0.0 (2026-03-02)
- **Major Release**: Complete feature polish and optimization
- Dynamic chunked copying (4KB-64MB speed-based)
- Speed test pre-scan with average calculations
- Full 52 error implementations in full-polish.cpp
- ImGui/Qt hybrid docking interface
- All tasks #1-10 completed
- CI tests 100% pass rate
- Release assets for all platforms (Linux, Windows, macOS)
- Screenshots and benchmarks included
- Professional documentation and tutorials

## v3.3.0 (2026-02-28)
- Release v3.3: Documentation update, rclone integration, hotkeys, tutorial PDF, CI badges

## v3.0.0 (2026-02-25)
- v1.1 Drives Detect: Enhanced DrivesTab with QFileSystemWatcher on /media/mnt, scan SD/USB with icons, eject functionality, DriveMonitor signals, test_drives.cpp added, C++17/Qt6
- Resolve merge conflicts for minimal version
- Add cross-platform build scripts, CI, full-polish.cpp with 128 cases, update README with badges

## v2.0.0 (2026-02-23)
- Migrated to Qt6 and CMake build system
- Added comprehensive test suite (128 cases)
- Enhanced CI/CD with Linux/Windows builds
- Improved UI with live preview graphs
- Parallel transfer optimization

## v1.0.0 (2026-02-22)
- Merge all feature branches to main
- Full modular architecture complete
- UI Tabs: Queue, Drive, Progress, Settings, AddTask
- Parallel transfers (4 threads), Live Hashing SHA256
- Drive auto-detect, Smart Queue, Error recovery
- Config live-reload, Progress tracking
- Unit tests for core modules
- Professional README + GitHub CI

## v0.9.x Feature Branches (merged)
- `feature-parallel-v2`: Multi-thread chunk copy
- `feature-settings`: SettingsDialog + persistence
- `feature-queuemanager`: Priority queue system
- `feature-hashmanager`: Thread-safe live hashing
- `feature-drivemonitor`: USB drive monitoring
- `feature-config-v7`: JSON/QSettings hybrid
- `feature-errorhandling`: Robust recovery
- `feature-live-preview-v3`: Graphics preview

## Earlier
See `git log --oneline main`

