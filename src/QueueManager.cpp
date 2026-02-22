#include &quot;QueueManager.h&quot;
#include &quot;TransferTask.h&quot; // Assume exists
#include &quot;Settings.h&quot;     // Assume exists for maxParallel
#include &lt;QDebug&gt;

QueueManager::QueueManager(QObject *parent)
    : QObject(parent), m_maxParallel(4) // Default
{
    connect(&amp;m_threadPool, &amp;QThreadPool::taskFinished, this, &amp;QueueManager::onTaskFinished);
    m_threadPool.setMaxThreadCount(m_maxParallel);
}

QueueManager::~QueueManager() {
    pauseAll();
    qDeleteAll(m_pendingQueue);
    qDeleteAll(m_activeTasks);
}

void QueueManager::addTask(TransferTask *task) {
    QMutexLocker locker(&amp;m_mutex);
    task-&gt;setStatus(TaskStatus::Pending);
    m_pendingQueue.enqueue(task);
    emit taskStatusChanged(task, TaskStatus::Pending);
    startNextTask();
}

void QueueManager::startNextTask() {
    QMutexLocker locker(&amp;m_mutex);
    while (m_pendingQueue.size() &gt; 0 &amp;&amp; m_activeTasks.size() &lt; static_cast&lt;size_t&gt;(m_maxParallel)) {
        TransferTask *task = m_pendingQueue.dequeue();
        task-&gt;setStatus(TaskStatus::Active);
        m_activeTasks.append(task);
        emit taskStatusChanged(task, TaskStatus::Active);
        // Start task in thread pool - assume TransferTask inherits QRunnable
        m_threadPool.start(task);
    }
}

void QueueManager::pauseAll() {
    QMutexLocker locker(&amp;m_mutex);
    for (TransferTask *task : m_activeTasks) {
        task-&gt;setStatus(TaskStatus::Paused);
        emit taskStatusChanged(task, TaskStatus::Paused);
    }
    m_threadPool.waitForDone();
}

void QueueManager::resumeAll() {
    QMutexLocker locker(&amp;m_mutex);
    for (TransferTask *task : m_activeTasks) {
        task-&gt;setStatus(TaskStatus::Active);
        emit taskStatusChanged(task, TaskStatus::Active);
        m_threadPool.start(task);
    }
    startNextTask();
}

QList&lt;TransferTask*&gt; QueueManager::getTasks() const {
    QMutexLocker locker(&amp;m_mutex);
    QList&lt;TransferTask*&gt; all;
    for (auto t : m_pendingQueue) all &lt;&lt; t;
    all &lt;&lt; m_activeTasks;
    return all;
}

int QueueManager::getMaxParallel() const {
    return m_maxParallel;
}

void QueueManager::setMaxParallel(int max) {
    QMutexLocker locker(&amp;m_mutex);
    m_maxParallel = max;
    m_threadPool.setMaxThreadCount(max);
}

void QueueManager::moveTask(int fromIndex, int toIndex) {
    QMutexLocker locker(&amp;m_mutex);
    // Simplified: combine pending + active for reordering
    QList&lt;TransferTask*&gt; allTasks;
    for (auto t : m_pendingQueue) allTasks &lt;&lt; t;
    allTasks &lt;&lt; m_activeTasks;
    
    if (fromIndex &gt;= 0 &amp;&amp; fromIndex &lt; allTasks.size() &amp;&amp;
        toIndex &gt;= 0 &amp;&amp; toIndex &lt; allTasks.size() &amp;&amp; fromIndex != toIndex) {
        TransferTask *moved = allTasks.takeAt(fromIndex);
        allTasks.insert(toIndex, moved);
        // Rebuild queues (pending first N, rest active - simplistic)
        m_pendingQueue.clear();
        for (int i = 0; i &lt; qMin(10, allTasks.size()); ++i) { // Arbitrary pending limit
            m_pendingQueue.enqueue(allTasks[i]);
        }
        m_activeTasks.clear();
        for (int i = m_pendingQueue.size(); i &lt; allTasks.size(); ++i) {
            m_activeTasks.append(allTasks[i]);
        }
        emit tasksReordered();
    }
}

void QueueManager::onTaskFinished() {
    QMutexLocker locker(&amp;m_mutex);
    // Remove completed tasks from active
    for (int i = m_activeTasks.size() - 1; i &gt;= 0; --i) {
        TransferTask *task = m_activeTasks[i];
        if (task-&gt;status() == TaskStatus::Completed) {
            m_activeTasks.removeAt(i);
            emit taskStatusChanged(task, TaskStatus::Completed);
            task-&gt;deleteLater(); // Or manage lifetime appropriately
        }
    }
    startNextTask();
}

void QueueManager::updateTaskStatus(TransferTask *task, TaskStatus status) {
    task-&gt;setStatus(status);
    emit taskStatusChanged(task, status);
}
