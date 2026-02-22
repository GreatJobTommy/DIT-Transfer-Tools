# Error Handling in DIT-Transfer-Tools

## ErrorManager

- **Error Categories**: DiskFull, TaskFail, DriveUnmount, NetworkError, HashMismatch, Unknown
- **Retry Logic**: 3 Versuche mit exponentieller Backoff (1s, 2s, 3s)
- **User Dialogs**: QMessageBox::critical mit Kategorie + Nachricht
- **Logging**: errors.log mit Timestamp + Category + Message

## Integration Points

### ChunkCopy (TransferTask::copyChunk)
```cpp
if (!errorMgr-&gt;handleError(ErrorCategory::DiskFull, "No space left on " + target)) {
    emit errorOccurred(ErrorCategory::DiskFull, "Disk full - copy aborted");
    return false;
}
```

### Queue (QueueManager::onTaskFinished)
```cpp
case TaskStatus::Failed:
    errorMgr-&gt;handleError(ErrorCategory::TaskFail, "Task " + task-&gt;source() + " failed");
    break;
```

### Drive (DriveMonitor::removeDrive)
```cpp
emit errorOccurred(ErrorCategory::DriveUnmount, path + " unmounted during transfer");
```

### MainWindow
- errorOccurred signal -> onErrorOccurred -> errorMgr-&gt;showUserDialog

## Tests
Siehe tests/test_errorhandling.cpp
