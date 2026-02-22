#ifndef QUEUEMANAGER_H
#define QUEUEMANAGER_H

#include &lt;QObject&gt;
#include &lt;QQueue&gt;
#include &lt;QThreadPool&gt;
#include &lt;QMutex&gt;
#include &lt;QWaitCondition&gt;

class TransferTask; // Forward declaration

enum class TaskStatus {
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
    QList&lt;TransferTask*&gt; getTasks() const;
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
    QQueue&lt;TransferTask*&gt; m_pendingQueue;
    QList&lt;TransferTask*&gt; m_activeTasks;
    mutable QMutex m_mutex;
    QWaitCondition m_condition;
    QThreadPool m_threadPool;
    int m_maxParallel;

    void updateTaskStatus(TransferTask *task, TaskStatus status);
};

#endif // QUEUEMANAGER_H
