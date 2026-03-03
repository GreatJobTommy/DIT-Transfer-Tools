#include "TransferTask.h"
#include <QFileInfo>
#include <QThread>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>
#include <QFile>
#include <QElapsedTimer>
#include <QDebug>
#include <QCoreApplication>\n#include <QRegularExpression>
#include <QStorageInfo>

TransferTask::TransferTask(const QString& source, const QString& destination, QObject* parent)
    : QObject(parent), QRunnable(), m_source(source), m_destination(destination), m_status(TransferStatus::Pending), m_finished(false), m_success(false), m_totalBytes(0), m_bytesTransferred(0),
      m_process(nullptr), m_retryTimer(nullptr), m_retryCount(0), m_maxRetries(5), m_backoffMs(1000), m_chunkSize(4096), m_lastBytes(0), m_hash(""), m_hashVerified(false) {
    QFileInfo fi(m_source);
    m_totalBytes = fi.exists() ? fi.size() : 0;

    QStorageInfo storageInfo(m_destination);
    if (storageInfo.isValid()) {
        m_isLTFS = storageInfo.fileSystemType().compare("ltfs", Qt::CaseInsensitive) == 0;
    }
    m_process = new QProcess(this);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &TransferTask::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred, this, &TransferTask::onProcessError);\n    connect(m_process, &QProcess::readyReadStandardOutput, this, &TransferTask::onReadyRead);
    m_retryTimer = new QTimer(this);
    connect(m_retryTimer, &QTimer::timeout, this, &TransferTask::retryTransfer);
    m_speedTimer.start();
    m_durationTimer.start();
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
        // Real local file copy with dynamic chunking
        QFile sourceFile(m_source);
        QFile destFile(m_destination);
        if (!sourceFile.open(QIODevice::ReadOnly)) {
            qWarning() << "Failed to open source file:" << m_source;
            m_finished = true;
            m_success = false;
            setStatus(TransferStatus::Failed);
            return;
        }
        QDir().mkpath(QFileInfo(m_destination).absolutePath());
        if (isLTFS()) {
            m_chunkSize = 64LL * 1024 * 1024; // Large sequential buffer for LTFS tapes
        }
        if (!destFile.open(QIODevice::WriteOnly)) {
            qWarning() << "Failed to open dest file:" << m_destination;
            sourceFile.close();
            m_finished = true;
            m_success = false;
            setStatus(TransferStatus::Failed);
            return;
        }

        m_speedTimer.restart();
        m_lastBytes = 0;
        qint64 speed = 0; // current speed
        char* buffer = new char[64 * 1024 * 1024]; // Max 64MB buffer

        while (!sourceFile.atEnd()) {
            qint64 bytesRead = sourceFile.read(buffer, m_chunkSize);
            if (bytesRead == -1) {
                qWarning() << "Read error";
                break;
            }
            qint64 bytesWritten = destFile.write(buffer, bytesRead);
            if (bytesWritten != bytesRead) {
                qWarning() << "Write error";
                break;
            }
            m_bytesTransferred += bytesRead;

            // Calculate speed and adjust chunk size
            qint64 elapsed = m_speedTimer.elapsed();
            if (elapsed > 1000) { // Every second
                qint64 bytesInSecond = m_bytesTransferred - m_lastBytes;
                speed = bytesInSecond * 1000 / elapsed; // bytes per second
                // Adjust chunk size: 4KB to 64MB based on speed
                if (speed < 1024 * 1024) { // <1MB/s
                    m_chunkSize = 4 * 1024; // 4KB
                } else if (speed < 10 * 1024 * 1024) { // <10MB/s
                    m_chunkSize = 64 * 1024; // 64KB
                } else if (speed < 100 * 1024 * 1024) { // <100MB/s
                    m_chunkSize = 1024 * 1024; // 1MB
                } else {
                    m_chunkSize = 64 * 1024 * 1024; // 64MB
                }
                m_lastBytes = m_bytesTransferred;
                m_speedTimer.restart();
            }

            qint64 remaining = m_totalBytes - m_bytesTransferred;
            qint64 eta = (speed > 0) ? remaining / speed : 0;
            emit progressChanged(m_bytesTransferred, speed, eta);

            QThread::yieldCurrentThread(); // Allow other threads
        }

        delete[] buffer;
        sourceFile.close();
        destFile.close();

        if (m_bytesTransferred == m_totalBytes) {
            m_finished = true;
            m_success = true;
            setStatus(TransferStatus::Completed);
        } else {
            m_finished = true;
            m_success = false;
            setStatus(TransferStatus::Failed);
        }
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

void TransferTask::onReadyRead() {
    QByteArray data = m_process->readAllStandardOutput();
    QString text = QString::fromUtf8(data);
    qDebug() << "Process output:" << text;
    // TODO: parse rsync/rclone progress JSON for progressChanged emit
}

void TransferTask::retryTransfer() {
    m_retryTimer->stop();
    run(); // Retry the transfer
}

int TransferTask::progress() const {
    if (m_totalBytes == 0) return 0;
    return (m_bytesTransferred * 100) / m_totalBytes;
}

qint64 TransferTask::remainingSize() const {
    return m_totalBytes - m_bytesTransferred;
}

qint64 TransferTask::eta() const {
    if (m_speedTimer.elapsed() == 0) return 0;
    qint64 speed = m_bytesTransferred / (m_speedTimer.elapsed() / 1000.0);
    if (speed == 0) return 0;
    return remainingSize() / speed;
}

QString TransferTask::hash() const {
    return m_hash;
}

bool TransferTask::hashVerified() const {
    return m_hashVerified;
}

qint64 TransferTask::duration() const {
    return m_durationTimer.elapsed() / 1000;
}

bool TransferTask::isLTFS() const {
    return m_isLTFS;
}