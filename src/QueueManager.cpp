#include "QueueManager.h"
#include "TransferTask.h" // Assume exists
#include "Settings.h"     // Assume exists for maxParallel
#include <QDebug>
QueueManager::QueueManager(QObject *parent)
    : QObject(parent), m_maxParallel(4) // Default
{
    connect(&m_threadPool, &QThreadPool::taskFinished, this, &QueueManager::onTaskFinished);
    m_threadPool.setMaxThreadCount(m_maxParallel);
}
QueueManager::~QueueManager() {
    pauseAll();
    qDeleteAll(m_pendingQueue);
    qDeleteAll(m_activeTasks);
}
void QueueManager::addTask(TransferTask *task) {
    QMutexLocker locker(&m_mutex);
    task->setStatus(TaskStatus::Pending);
    m_pendingQueue.enqueue(task);
    emit taskStatusChanged(task, TaskStatus::Pending);
    startNextTask();
}
void QueueManager::startNextTask() {
    QMutexLocker locker(&m_mutex);
    while (m_pendingQueue.size() > 0 && m_activeTasks.size() < static_cast<size_t>(m_maxParallel)) {
        TransferTask *task = m_pendingQueue.dequeue();
        task->setStatus(TaskStatus::Active);
        m_activeTasks.append(task);
        emit taskStatusChanged(task, TaskStatus::Active);
        // Start task in thread pool - assume TransferTask inherits QRunnable
        m_threadPool.start(task);
    }
}
void QueueManager::pauseAll() {
    QMutexLocker locker(&m_mutex);
    for (TransferTask *task : m_activeTasks) {
        task->setStatus(TaskStatus::Paused);
        emit taskStatusChanged(task, TaskStatus::Paused);
    }
    m_threadPool.waitForDone();
}
void QueueManager::resumeAll() {
    QMutexLocker locker(&m_mutex);
    for (TransferTask *task : m_activeTasks) {
        task->setStatus(TaskStatus::Active);
        emit taskStatusChanged(task, TaskStatus::Active);
        m_threadPool.start(task);
    }
    startNextTask();
}
QList<TransferTask*> QueueManager::getTasks() const {
    QMutexLocker locker(&m_mutex);
    QList<TransferTask*> all;
    for (auto t : m_pendingQueue) all << t;
    all << m_activeTasks;
    return all;
}
int QueueManager::getMaxParallel() const {
    return m_maxParallel;
}
void QueueManager::setMaxParallel(int max) {
    QMutexLocker locker(&m_mutex);
    m_maxParallel = max;
    m_threadPool.setMaxThreadCount(max);
}
void QueueManager::moveTask(int fromIndex, int toIndex) {
    QMutexLocker locker(&m_mutex);
    // Simplified: combine pending + active for reordering
    QList<TransferTask*> allTasks;
    for (auto t : m_pendingQueue) allTasks << t;
    allTasks << m_activeTasks;
    
    if (fromIndex >= 0 && fromIndex < allTasks.size() &&
        toIndex >= 0 && toIndex < allTasks.size() && fromIndex != toIndex) {
        TransferTask *moved = allTasks.takeAt(fromIndex);
        allTasks.insert(toIndex, moved);
        // Rebuild queues (pending first N, rest active - simplistic)
        m_pendingQueue.clear();
        for (int i = 0; i < qMin(10, allTasks.size()); ++i) { // Arbitrary pending limit
            m_pendingQueue.enqueue(allTasks[i]);
        }
        m_activeTasks.clear();
        for (int i = m_pendingQueue.size(); i < allTasks.size(); ++i) {
            m_activeTasks.append(allTasks[i]);
        }
        emit tasksReordered();
    }
}
void QueueManager::onTaskFinished() {
    QMutexLocker locker(&m_mutex);
    // Remove completed tasks from active
    for (int i = m_activeTasks.size() - 1; i >= 0; --i) {
        TransferTask *task = m_activeTasks[i];
        if (task->status() == TaskStatus::Completed) {
            m_activeTasks.removeAt(i);
            emit taskStatusChanged(task, TaskStatus::Completed);
            task->deleteLater(); // Or manage lifetime appropriately
        }
    }
    startNextTask();
}
void QueueManager::updateTaskStatus(TransferTask *task, TaskStatus status) {
    task->setStatus(status);
    emit taskStatusChanged(task, status);
}
