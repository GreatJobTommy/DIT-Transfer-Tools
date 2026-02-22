#ifndef PARALLELMANAGER_H
#define PARALLELMANAGER_H

#include &lt;QObject&gt;
#include &lt;QThreadPool&gt;
#include &lt;QMutex&gt;
#include &lt;QPriorityQueue&gt; // C++11, but Qt has no built-in, use std::priority_queue
#include &lt;queue&gt;
#include &lt;TransferTask.h&gt; // Assume exists
#include &lt;functional&gt;

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
        bool operator&lt;(const TaskEntry&amp; other) const { return priority &lt; other.priority; }
    };
    std::priority_queue&lt;TaskEntry&gt; m_taskQueue;
    QList&lt;TransferTask*&gt; m_activeTasks;
    mutable QMutex m_mutex;
    QThreadPool m_threadPool;
    int m_maxThreads;
    bool m_paused;

    void startNextTask();
    void loadBalance();
};

#endif // PARALLELMANAGER_H
