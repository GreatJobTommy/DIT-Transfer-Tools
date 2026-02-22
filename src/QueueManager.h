#ifndef QUEUEMANAGER_H
#define QUEUEMANAGER_H
#include <QObject>
#include <QQueue>
#include <QThreadPool>
#include <QMutex>
#include <QWaitCondition>
#include "ErrorManager.h"
class TransferTask; // Forward declaration
enum class TaskStatus {
    Pending,
    Active,
    Paused,
    Completed,
    Failed
};
    Pending,
    Active,
    Paused,
    Completed
};
class QueueManager : public QObject {
    Q_OBJECT
public:
    explicit QueueManager(QObject *parent = nullptr);
    ~QueueManager();
    void addTask(TransferTask *task);
    void startNextTask();
    void pauseAll();
    void resumeAll();
    QList<TransferTask*> getTasks() const;
    int getMaxParallel() const;
    void setMaxParallel(int max);
    // Drag-reorder (basic support, emit signal for UI to handle actual reordering)
    void moveTask(int fromIndex, int toIndex);
signals:
    void taskStatusChanged(TransferTask *task, TaskStatus status);
    void tasksReordered();
private slots:
    void onTaskFinished();
private:
    QQueue<TransferTask*> m_pendingQueue;
    QList<TransferTask*> m_activeTasks;
    mutable QMutex m_mutex;
    QWaitCondition m_condition;
    QThreadPool m_threadPool;
    int m_maxParallel;
    void updateTaskStatus(TransferTask *task, TaskStatus status);
};
#endif // QUEUEMANAGER_H
