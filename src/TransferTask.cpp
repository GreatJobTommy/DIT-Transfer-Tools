#include "TransferTask.h"

TransferTask::TransferTask(const QString& source, const QString& destination, QObject* parent)
    : QObject(parent), m_source(source), m_destination(destination), m_status(TransferStatus::Pending) {}

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
    emit statusChanged(status);  // Assuming signal, but not defined yet
}

// Need to add signal in .h if used
// signals:
//     void statusChanged(TransferStatus status);