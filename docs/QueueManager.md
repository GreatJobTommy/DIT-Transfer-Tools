# QueueManager Dokumentation

## Overview
QueueManager verwaltet TransferTasks mit QQueue für pending und QThreadPool für parallele Ausführung.

## Features
- **Status**: Pending, Active, Paused, Completed
- **maxParallel**: Aus Settings (default 4)
- **Drag-Reorder**: moveTask(from, to) - kombiniert pending/active Liste, rebuilds queues
- **Integration**:
  - TransferTask: QRunnable mit HashManager für Integrität
  - HashManager: Sha256 Hash compute/verify

## Usage
```cpp
QueueManager *qm = new QueueManager();
TransferTask *task = new TransferTask();
qm-&gt;addTask(task);
qm-&gt;setMaxParallel(Settings::maxParallelTransfers());
```

## Signals
- taskStatusChanged(TransferTask*, TaskStatus)
- tasksReordered()

## Tests
`qmake &amp;&amp; make &amp;&amp; ./test_queuemanager`

**Drag-Reorder Hinweis**: UI muss Indexe tracken, Signal für Update.
