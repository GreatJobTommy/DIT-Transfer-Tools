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

    // Emit notification signals
    switch (status) {
        case TransferStatus::Completed:
            emit taskCompleted(task);
            break;
        case TransferStatus::Failed:
            emit taskFailed(task);
            break;
        case TransferStatus::Paused:
            emit taskPaused(task);
            break;
        default:
            break;
    }

    if (status == TransferStatus::Completed || status == TransferStatus::Failed) {
        m_active.removeOne(task);
        activateNext();
    }
}

void QueueManager::activateNext() {
    while (m_active.size() < m_maxActive && !m_waiting.isEmpty()) {
        TransferTask* task = m_waiting.takeFirst();
        task->setStatus(TransferStatus::Active);
        m_active.append(task);
    }
}