# DriveMonitor Dokumentation

## Overview
DriveMonitor überwacht USB/Laufwerke cross-platform mit **QStorageInfo** (Qt6+) und **QFileSystemWatcher** (Echtzeit) + Fallback-Polling (5s).

**Features:**
- Cross-Platform: Linux (`/media`), Windows (`wmic`), macOS
- Auto-Detect Connect/Disconnect
- **Auto-Pause QueueManager** bei Disconnect
- **Auto-Resume** bei Reconnect
- Signalisierung an MainWindow/Queue

## API

### DriveMonitor.h
```cpp
class DriveMonitor : public QObject {
    Q_OBJECT
signals:
    void driveConnected(const QString &amp;path);
    void driveDisconnected(const QString &amp;path);
    void drivesChanged(const QStringList &amp;available);
};
```

### Integration MainWindow.cpp
```cpp
connect(driveMon, &amp;DriveMonitor::driveDisconnected, queueMgr, &amp;QueueManager::pauseAll);
connect(driveMon, &amp;DriveMonitor::driveConnected, queueMgr, &amp;QueueManager::resumeAll);
```

## Implementation Details

### Cross-Platform Detection
```cpp
QList&lt;QStorageInfo&gt; drives = QStorageInfo::mountedVolumes();
for (const QStorageInfo &amp;info : drives) {
    if (info.isReady() &amp;&amp; isRemovableDrive(info)) {
        // Register watcher
    }
}
```

**isRemovableDrive**: Prüft `info.device()` auf USB-Keywords oder Größe &lt; 1TB.

### Echtzeit + Fallback
1. **QFileSystemWatcher** auf `/media/*` (Linux), `D:\\` etc.
2. **QTimer** 5s Poll als Fallback (battery-friendly).

### Queue Integration
- **Disconnect**: `queueMgr-&gt;pauseAll()`
- **Reconnect**: `queueMgr-&gt;resumeAll()`
- Tasks mit Drive-Pfad pausieren selektiv.

## Tests
`tests/test_drivemonitor.cpp`:
- `testQStorageInfoCrossPlatform()`
- `testPollingFallback()`
- `testQueuePauseResume()`

## Usage
```
DriveMonitor *mon = new DriveMonitor(this);
connect(mon, &amp;DriveMonitor::drivesChanged, ui-&gt;driveList, &amp;QListWidget::clear);
```

**LoC**: ~180 (DriveMonitor.cpp/h), Tests: ~80, Docs: diese Datei.

**Status**: Vollständig implementiert, getestet auf Linux/RPi.