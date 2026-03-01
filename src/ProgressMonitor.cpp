#include "ProgressMonitor.h"
#include <QDateTime>

ProgressMonitor::ProgressMonitor(QObject *parent) : QObject(parent), m_totalBytes(0), m_transferred(0) {}

ProgressMonitor::~ProgressMonitor() {}

void ProgressMonitor::startTransfer(qint64 totalBytes) {
    m_totalBytes = totalBytes;
    m_transferred = 0;
    m_timer.start();
    m_logs.clear();
    QString log = QString("Started transfer: %1 bytes").arg(totalBytes);
    m_logs.append(QDateTime::currentDateTime().toString() + ": " + log);
    emit progressUpdated(0, 0, log);
}

void ProgressMonitor::updateProgress(qint64 bytesTransferred) {
    m_transferred = bytesTransferred;
    qint64 elapsed = m_timer.elapsed();
    double speed = (elapsed > 0) ? (double)m_transferred / elapsed * 1000.0 / (1024*1024) : 0; // MB/s
    qint64 remaining = m_totalBytes - m_transferred;
    qint64 eta = (speed > 0) ? remaining / (speed * 1024 * 1024) : 0; // seconds

    QString log = QString("Progress: %1/%2 bytes, Speed: %3 MB/s, ETA: %4 s").arg(bytesTransferred).arg(m_totalBytes).arg(speed, 0, 'f', 2).arg(eta);
    m_logs.append(QDateTime::currentDateTime().toString() + ": " + log);
    emit progressUpdated(speed, eta, log);
}

void ProgressMonitor::finishTransfer() {
    QString log = "Transfer finished";
    m_logs.append(QDateTime::currentDateTime().toString() + ": " + log);
    emit progressUpdated(0, 0, log);
}

double ProgressMonitor::currentSpeed() const {
    qint64 elapsed = m_timer.elapsed();
    return (elapsed > 0) ? (double)m_transferred / elapsed * 1000.0 / (1024*1024) : 0;
}

qint64 ProgressMonitor::eta() const {
    double speed = currentSpeed();
    qint64 remaining = m_totalBytes - m_transferred;
    return (speed > 0) ? remaining / (speed * 1024 * 1024) : 0;
}

QString ProgressMonitor::logs() const {
    return m_logs.join("\n");
}