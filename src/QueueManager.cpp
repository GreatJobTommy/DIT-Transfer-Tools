#include "QueueManager.h"

QueueManager::QueueManager(int maxActive, QObject* parent)
    : QObject(parent), m_maxActive(maxActive) {}

QueueManager::~QueueManager() {
    qDeleteAll(m_active);
    qDeleteAll(m_waiting);
}

void QueueManager::addTask(TransferTask* task) {
    if (!task) return;
    connect(task, &TransferTask::statusChanged, this, &QueueManager::onTaskStatusChanged);
    connect(task, &TransferTask::progressChanged, this, &QueueManager::onTaskProgressChanged);
    m_waiting.append(task);
    activateNext();
}

void QueueManager::removeTask(TransferTask* task) {
    if (!task) return;
    disconnect(task, &TransferTask::statusChanged, this, &QueueManager::onTaskStatusChanged);
    m_active.removeOne(task);
    m_waiting.removeOne(task);
    delete task;
    activateNext();
}

void QueueManager::reorderWaiting(int from, int to) {
    if (from < 0 || from >= m_waiting.size() || to < 0 || to >= m_waiting.size()) return;
    m_waiting.move(from, to);
}

void QueueManager::setWaitingOrder(const QList<TransferTask*>& order) {
    m_waiting = order;
}

QList<TransferTask*> QueueManager::activeTasks() const {
    return m_active;
}

QList<TransferTask*> QueueManager::waitingTasks() const {
    return m_waiting;
}

int QueueManager::maxActive() const {
    return m_maxActive;
}

void QueueManager::setMaxActive(int max) {
    m_maxActive = max;
    activateNext();
}

void QueueManager::onTaskStatusChanged(TransferStatus status) {
    TransferTask* task = qobject_cast<TransferTask*>(sender());
    if (!task) return;
    if (status == TransferStatus::Completed || status == TransferStatus::Failed) {
        m_active.removeOne(task);
        activateNext();
    }
}

void QueueManager::onTaskProgressChanged(const QString& dest, qint64 bytes, qint64 speed, qint64 eta) {
    emit progressChanged(dest, bytes, speed, eta);
    // Calculate overall
    qint64 totalBytes = 0;
    qint64 totalTransferred = 0;
    qint64 totalSpeed = 0;
    for (TransferTask* task : m_active) {
        totalBytes += task->totalBytes() * task->destinations().size();
        for (qint64 b : task->bytesTransferredPerDest().values()) {
            totalTransferred += b;
        }
        // Speed per dest, but approximate
        totalSpeed += speed * task->destinations().size();
    }
    emit overallProgressChanged(totalBytes, totalTransferred, totalSpeed);
}

void QueueManager::activateNext() {
    while (m_active.size() < m_maxActive && !m_waiting.isEmpty()) {
        TransferTask* task = m_waiting.takeFirst();
        task->setStatus(TransferStatus::Active);
        m_active.append(task);
    }
}