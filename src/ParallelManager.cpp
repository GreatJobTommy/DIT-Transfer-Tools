#include "ParallelManager.h"
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QtGlobal>

ParallelManager::ParallelManager(QObject *parent)
    : QObject(parent), m_maxThreads(4), m_paused(false)
{
    m_threadPool.setMaxThreadCount(m_maxThreads);
    // Connect if needed for task finished signal
}

ParallelManager::~ParallelManager() {
    pause();
    m_threadPool.waitForDone();
}

void ParallelManager::enqueueTask(TransferTask* task, int priority) {
    QMutexLocker lock(&m_mutex);
    m_taskQueue.push({task, priority});
    if (!m_paused) startNextTask();
}

void ParallelManager::setMaxThreads(int threads) {
    QMutexLocker lock(&m_mutex);
    m_maxThreads = qMax(1, qMin(32, threads));
    m_threadPool.setMaxThreadCount(m_maxThreads);
    loadBalance();
}

int ParallelManager::maxThreads() const {
    return static_cast<int>(m_maxThreads);
}

void ParallelManager::pause() {
    QMutexLocker lock(&m_mutex);
    m_paused = true;
}

void ParallelManager::resume() {
    QMutexLocker lock(&m_mutex);
    m_paused = false;
    startNextTask();
}

int ParallelManager::activeThreads() const {
    return m_threadPool.activeThreadCount();
}

void ParallelManager::startNextTask() {
    QMutexLocker lock(&m_mutex);
    while (!m_taskQueue.empty() && m_activeTasks.size() < m_maxThreads && !m_paused) {
        auto entry = m_taskQueue.top(); m_taskQueue.pop();
        m_activeTasks.append(entry.task);
        emit taskStarted(entry.task);
        m_threadPool.start(entry.task); // Assume TransferTask is QRunnable
    }
}

void ParallelManager::onTaskFinished() {
    QMutexLocker lock(&m_mutex);
    // Note: This slot assumes connection to thread pool or task signal
    for (int i = m_activeTasks.size() - 1; i >= 0; --i) {
        TransferTask* task = m_activeTasks[i];
        if (task->isFinished()) { // Assume method
            m_activeTasks.removeAt(i);
            emit taskFinished(task, task->success());
            startNextTask();
            break;
        }
    }
}

void ParallelManager::loadBalance() {
    // Simple load balance: adjust based on CPU? Placeholder
    qDebug() << "Load balanced to" << m_maxThreads << "threads";
}
