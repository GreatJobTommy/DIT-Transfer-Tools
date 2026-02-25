#ifndef PARALLELMANAGER_H
#define PARALLELMANAGER_H

#include <QObject>
#include <QThreadPool>
#include <QMutex>
// Qt has no QPriorityQueue, use std::priority_queue
#include <queue>
#include "TransferTask.h" // Assume exists
#include <functional>

class ParallelManager : public QObject {
    Q_OBJECT

public:
    explicit ParallelManager(QObject *parent = nullptr);
    ~ParallelManager();

    void enqueueTask(TransferTask* task, int priority = 0); // Higher number = higher priority
    void setMaxThreads(int threads);
    int maxThreads() const;
    void pause();
    void resume();
    int activeThreads() const;

signals:
    void taskStarted(TransferTask* task);
    void taskFinished(TransferTask* task, bool success);

private slots:
    void onTaskFinished();

private:
    struct TaskEntry {
        TransferTask* task;
        int priority;
        bool operator<(const TaskEntry& other) const { return priority < other.priority; }
    };
    std::priority_queue<TaskEntry> m_taskQueue;
    QList<TransferTask*> m_activeTasks;
    mutable QMutex m_mutex;
    QThreadPool m_threadPool;
    int m_maxThreads;
    bool m_paused;

    void startNextTask();
    void loadBalance();
};

#endif // PARALLELMANAGER_H
