#include "TransferTask.h"
#include <QFileInfo>
#include <QThread>
#include <QFile>
#include <QDir>

TransferTask::TransferTask(const QString& source, const QMap<QString, QString>& destinations, QObject* parent)
    : QObject(parent), QRunnable(), m_source(source), m_destinations(destinations), m_status(TransferStatus::Pending), m_finished(false), m_success(false), m_totalBytes(0) {
    QFileInfo fi(m_source);
    m_totalBytes = fi.exists() ? fi.size() : 1000000;
    for (const QString& dest : m_destinations.values()) {
        m_bytesTransferredPerDest[dest] = 0;
    }
}

TransferTask::~TransferTask() {}

QString TransferTask::source() const {
    return m_source;
}

QMap<QString, QString> TransferTask::destinations() const {
    return m_destinations;
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

qint64 TransferTask::bytesTransferred(const QString& dest) const {
    if (dest.isEmpty()) {
        qint64 total = 0;
        for (qint64 bytes : m_bytesTransferredPerDest.values()) {
            total += bytes;
        }
        return total / m_destinations.size(); // Average or something, but for simplicity
    }
    return m_bytesTransferredPerDest.value(dest, 0);
}

QMap<QString, qint64> TransferTask::bytesTransferredPerDest() const {
    return m_bytesTransferredPerDest;
}

void TransferTask::run() {
    setStatus(TransferStatus::Active);
    qint64 chunk = 10240; // 10KB per dest
    qint64 speed = 102400; // 100KB/s per dest

    bool allDone = false;
    while (!allDone) {
        allDone = true;
        for (auto it = m_destinations.begin(); it != m_destinations.end(); ++it) {
            const QString& dest = it.value();
            if (m_bytesTransferredPerDest[dest] < m_totalBytes) {
                allDone = false;
                QThread::msleep(10); // Simulate time
                m_bytesTransferredPerDest[dest] += chunk;
                if (m_bytesTransferredPerDest[dest] > m_totalBytes) m_bytesTransferredPerDest[dest] = m_totalBytes;
                qint64 remaining = m_totalBytes - m_bytesTransferredPerDest[dest];
                qint64 eta = (remaining > 0) ? remaining / speed : 0;
                emit progressChanged(it.key(), m_bytesTransferredPerDest[dest], speed, eta);
            }
        }
    }
    m_finished = true;
    m_success = true;
    setStatus(TransferStatus::Completed);
}