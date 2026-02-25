#include "TransferTask.h"

TransferTask::TransferTask(const QString& source, const QString& destination, QObject* parent)
    : QObject(parent), QRunnable(), m_source(source), m_destination(destination), m_status(TransferStatus::Pending), m_finished(false), m_success(false) {}

TransferTask::~TransferTask() {}

QString TransferTask::source() const {
    return m_source;
}

QString TransferTask::destination() const {
    return m_destination;
}

TransferStatus TransferTask::status() const {
    return m_status;
}

void TransferTask::setStatus(TransferStatus status) {
    m_status = status;
    emit statusChanged(status);
}

bool TransferTask::isFinished() const {
    return m_finished;
}

bool TransferTask::success() const {
    return m_success;
}

void TransferTask::run() {
    // Simulate transfer
    setStatus(TransferStatus::Active);
    // Do work here
    m_finished = true;
    m_success = true; // Assume success
    setStatus(TransferStatus::Completed);
}