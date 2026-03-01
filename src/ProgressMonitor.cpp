#include "ProgressMonitor.h"
#include "TransferTask.h"
#include <QDebug>

ProgressMonitor::ProgressMonitor(QObject* parent)
    : QObject(parent) {}

ProgressMonitor::~ProgressMonitor() {}

void ProgressMonitor::addTask(TransferTask* task) {
    if (!m_progress.contains(task)) {
        m_progress[task] = 0;
        connect(task, &TransferTask::progressChanged, this, [this, task](qint64 bytes, qint64 speed, qint64 eta) {
            int percent = (task->totalBytes() > 0) ? (bytes * 100) / task->totalBytes() : 0;
            m_progress[task] = percent;
            emit progressUpdated(task, percent);
            if (percent == 100) {
                emit taskCompleted(task);
            }
        });
    }
}

void ProgressMonitor::removeTask(TransferTask* task) {
    m_progress.remove(task);
    disconnect(task, nullptr, this, nullptr);
}

QMap<TransferTask*, int> ProgressMonitor::getProgress() const {
    return m_progress;
}

QString ProgressMonitor::getLogs() const {
    return m_logs;
}