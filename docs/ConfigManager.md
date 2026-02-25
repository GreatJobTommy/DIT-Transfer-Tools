# ConfigManager Dokumentation

## Overview
ConfigManager is a singleton class that manages all application settings using QSettings for primary storage and JSON backup for portability. Features live reload via QFileSystemWatcher and automatic recovery from corrupt JSON files.

## Key Features
- **QSettings + JSON Backup**: Primary INI storage with JSON export/import
- **Live Reload**: Automatic reload when config.json changes externally
- **Signals**: Emits signals on setting changes for UI updates
- **Defaults & Migration**: Built-in defaults and old format migration
- **Corruption Recovery**: Detects invalid JSON and falls back to defaults

## Usage
```cpp
ConfigManager *mgr = ConfigManager::instance();
mgr-&gt;setTransferDir(&quot;/mnt/usb&quot;);
connect(mgr, &amp;ConfigManager::transferDirChanged, this, &amp;MyClass::onTransferDirChanged);
```

## Settings API
- `setTransferDir(QString)` / `getTransferDir()`
- `setHashAlgorithm(QString)` / `getHashAlgorithm()`
- `setAutoTransfer(bool)` / `getAutoTransfer()`

## Storage Locations
- QSettings: Standard app locations
- JSON Backup: `$XDG_CONFIG_HOME/DIT-Transfer-Tools/config.json`

## Recovery Mechanism
1. Attempt JSON load
2. If invalid, log &quot;Recovering from corrupt JSON&quot; and load defaults
3. Maintain QSettings as fallback

## Tests
- `testDefaults()`: Verifies default values
- `testSaveLoad()`: Roundtrip save/load
- `testCorruptJsonRecovery()`: Corruption handling
