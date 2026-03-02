#include "TransferTask.h"
#include <QFileInfo>
#include <QThread>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>

TransferTask::TransferTask(const QString& source, const QString& destination, QObject* parent)
    : QObject(parent), QRunnable(), m_source(source), m_destination(destination), m_status(TransferStatus::Pending), m_finished(false), m_success(false), m_totalBytes(0), m_bytesTransferred(0),
      m_process(nullptr), m_retryTimer(nullptr), m_retryCount(0), m_maxRetries(5), m_backoffMs(1000) {
    QFileInfo fi(m_source);
    m_totalBytes = fi.exists() ? fi.size() : 1000000;
    m_process = new QProcess(this);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &TransferTask::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred, this, &TransferTask::onProcessError);
    m_retryTimer = new QTimer(this);
    connect(m_retryTimer, &QTimer::timeout, this, &TransferTask::retryTransfer);
}

TransferTask::~TransferTask() {
    if (m_process) {
        m_process->kill();
        m_process->waitForFinished(3000);
    }
}

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

bool TransferTask::isRcloneRemote() const {
    return m_destination.contains(':');
}

void TransferTask::run() {
    setStatus(TransferStatus::Active);
    if (isRcloneRemote()) {
        // Use rclone
        QString rclonePath = "rclone"; // Assume in PATH, or get from settings
        QSettings settings;
        QString configPath = settings.value("rclone/configPath", QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/rclone/rclone.conf").toString();
        QStringList args;
        args << "copy" << "--checksum" << "--config" << configPath << m_source << m_destination;
        m_process->start(rclonePath, args);
    } else {
        // Local copy simulation
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
}

void TransferTask::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
        m_finished = true;
        m_success = true;
        setStatus(TransferStatus::Completed);
    } else {
        if (m_retryCount < m_maxRetries) {
            m_retryCount++;
            m_backoffMs *= 2; // Exponential backoff
            m_retryTimer->start(m_backoffMs);
        } else {
            m_finished = true;
            m_success = false;
            setStatus(TransferStatus::Failed);
        }
    }
}

void TransferTask::onProcessError(QProcess::ProcessError error) {
    // Handle process error, maybe retry
    if (m_retryCount < m_maxRetries) {
        m_retryCount++;
        m_backoffMs *= 2;
        m_retryTimer->start(m_backoffMs);
    } else {
        m_finished = true;
        m_success = false;
        setStatus(TransferStatus::Failed);
    }
}

void TransferTask::retryTransfer() {
    m_retryTimer->stop();
    run(); // Retry the transfer
}