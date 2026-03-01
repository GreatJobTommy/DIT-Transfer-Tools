#include "TransferTask.h"
#include <QFileInfo>
#include <QThread>

TransferTask::TransferTask(const QString& source, const QString& destination, QObject* parent)
    : QObject(parent), QRunnable(), m_source(source), m_destination(destination), m_status(TransferStatus::Pending), m_finished(false), m_success(false), m_totalBytes(0), m_bytesTransferred(0) {
    QFileInfo fi(m_source);
    m_totalBytes = fi.exists() ? fi.size() : 1000000;
}

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

qint64 TransferTask::totalBytes() const {
    return m_totalBytes;
}

qint64 TransferTask::bytesTransferred() const {
    return m_bytesTransferred;
}

void TransferTask::run() {
    setStatus(TransferStatus::Active);
    qint64 chunk = 10240; // 10KB
    qint64 speed = 102400; // 100KB/s
    while (m_bytesTransferred < m_totalBytes) {
        QThread::msleep(100); // Simulate time
        m_bytesTransferred += chunk;
        if (m_bytesTransferred > m_totalBytes) m_bytesTransferred = m_totalBytes;
        qint64 remaining = m_totalBytes - m_bytesTransferred;
        qint64 eta = (remaining > 0) ? remaining / speed : 0;
        emit progressChanged(m_bytesTransferred, speed, eta);
    }
    m_finished = true;
    m_success = true;
    setStatus(TransferStatus::Completed);
}