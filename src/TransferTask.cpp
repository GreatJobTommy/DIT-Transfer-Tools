#include "TransferTask.h"
#include <QFileInfo>
#include <QThread>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>
#include <QFile>
#include <QElapsedTimer>
#include <QDebug>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QStorageInfo>
#include <QCryptographicHash>
#include <QDirIterator>
#include "HashManager.h"
#include <QRandomGenerator>
#include <algorithm>

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
    connect(m_process, &QProcess::errorOccurred, this, &TransferTask::onProcessError);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &TransferTask::onReadyRead);
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
    return m_source.startsWith("rclone://") || m_destination.startsWith("rclone://");
}

void TransferTask::run() {
    setStatus(TransferStatus::Active);
    if (isRcloneRemote()) {
        // Use rclone
        QSettings settings;
        QString configPath = settings.value("rclone/configPath", QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/rclone/rclone.conf").toString();
        QString srcPath = m_source.startsWith("rclone://") ? m_source.mid(9) : m_source;
        QString dstPath = m_destination.startsWith("rclone://") ? m_destination.mid(9) : m_destination;
        QStringList args;
        args << "copy" << srcPath << dstPath << "--progress" << "--stats" << "1s" << "--retries" << "3" << "--low-level-retries" << "10" << "--transfers" << QString::number(QThread::idealThreadCount()) << "--checksum" << "--config" << configPath;
        if (!m_preset.isEmpty()) {
            if (m_preset == "S3 Fast") {
                args << "--s3-upload-concurrency" << "8" << "--s3-chunk-size" << "5G";
            } else if (m_preset == "GDrive") {
                args << "--drive-use-trash" << "false" << "--drive-keep-revision-forever" << "true";
            } else if (m_preset == "Safe Copy") {
                args << "--checksum-mode" << "sha256";
            }
        }
        m_process->setProcessChannelMode(QProcess::MergedChannels);
        m_process->start("rclone", args);
    } else {
        QFileInfo srcInfo(m_source);
        if (srcInfo.isDir() || m_isLTFS) {
            QStringList args;
            args << "-a" << "--whole-file" << "--info=progress2" << m_source << m_destination;
            m_process->setProcessChannelMode(QProcess::MergedChannels);
            m_process->start("rsync", args);
            return;
        }
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

        m_success = verifyTransfer();
        m_finished = true;
        setStatus(m_success ? TransferStatus::Completed : TransferStatus::Failed);
    }
}

void TransferTask::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
        m_success = verifyTransfer();
        m_finished = true;
        setStatus(m_success ? TransferStatus::Completed : TransferStatus::Failed);
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
    QString output = QString::fromUtf8(m_process->readAllStandardOutput());
    qDebug() << "Process output:" << output;
    QRegularExpression re(R"(Transferred:\\s+([\\d.]+)\\s+([KGTM]i?B)\\s*/\\s+([\\d.]+)\\s+([KGTM]i?B),\\s+(\\d+)%?,\\s+([\\d.]+)\\s+([KGTM]i?B)/s(?:,\\s+ETA\\s+(.+))?)");
    QRegularExpressionMatch match = re.match(output);
    if (match.hasMatch()) {
        qint64 done = parseSize(match.captured(1) + match.captured(2));
        qint64 total = parseSize(match.captured(3) + match.captured(4));
        qint64 speed = parseSize(match.captured(6) + match.captured(7));
        QString etaStr = match.captured(8);
        qint64 eta = parseEta(etaStr);
        if (total > 0) m_totalBytes = total;
        m_bytesTransferred = done;
        emit progressChanged(done, speed, eta);
        qDebug() << "Parsed rclone:" << done << "/" << total << speed << eta;
    }
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

qint64 TransferTask::parseSize(const QString &sizeStr) {
    QString s = sizeStr.trimmed();
    if (s.isEmpty()) return 0;
    QRegularExpression re(R"(^([\\d.]+)\\s*([KMBTG]i?b?)$)");
    QRegularExpressionMatch m = re.match(s);
    if (!m.hasMatch()) {
        return s.replace(",","").toDouble();
    }
    double num = m.captured(1).toDouble();
    QString unit = m.captured(2).toUpper();
    qint64 mult = 1;
    QChar first = unit[0];
    if (first == 'K') mult = 1024LL;
    else if (first == 'M') mult = 1024LL * 1024;
    else if (first == 'G') mult = 1024LL * 1024 * 1024LL;
    else if (first == 'T') mult = 1024LL * 1024 * 1024LL * 1024LL;
    return qint64(num * mult);
}

qint64 TransferTask::parseEta(const QString &etaStr) {
    if (etaStr.trimmed().isEmpty()) return 0;
    QString s = etaStr.trimmed().toLower();
    qint64 secs = 0;
    int posH = s.indexOf('h');
    if (posH != -1) {
        secs += s.left(posH).toInt() * 3600;
        s = s.mid(posH + 1);
    }
    int posM = s.indexOf('m');
    if (posM != -1) {
        secs += s.left(posM).toInt() * 60;
        s = s.mid(posM + 1);
    }
    int posS = s.indexOf('s');
    if (posS != -1) {
        secs += s.left(posS).toInt();
    } else {
        secs += s.toInt();
    }
    return secs;
}
bool TransferTask::verifyTransfer() {
    if (!m_verifyEnabled) return true;

    QFileInfo srcInfo(m_source);
    if (!srcInfo.isDir()) {
        // Single file copy already does basic check
        return true;
    }

    QList<FileInfo> srcFiles = getAllFiles(m_source);
    QList<FileInfo> destFiles = getAllFiles(m_destination);

    if (srcFiles.size() != destFiles.size()) {
        qWarning() << "File count mismatch:" << srcFiles.size() << "/" << destFiles.size();
        return false;
    }

    // Sort by relPath
    std::sort(srcFiles.begin(), srcFiles.end(), [](const FileInfo& a, const FileInfo& b){
        return a.relPath < b.relPath;
    });
    std::sort(destFiles.begin(), destFiles.end(), [](const FileInfo& a, const FileInfo& b){
        return a.relPath < b.relPath;
    });

    // Size checks
    for (int i = 0; i < srcFiles.size(); ++i) {
        if (srcFiles[i].relPath != destFiles[i].relPath || srcFiles[i].size != destFiles[i].size) {
            qWarning() << "Mismatch:" << srcFiles[i].relPath << srcFiles[i].size << destFiles[i].size;
            return false;
        }
    }

    // Spot-check 5 random large files (>5MB)
    QList<FileInfo> largeFiles;
    for (const auto& f : srcFiles) {
        if (f.size > 5LL * 1024 * 1024) largeFiles.append(f);
    }

    if (!largeFiles.isEmpty()) {
        QRandomGenerator* rng = QRandomGenerator::global();
        int numChecks = qMin(5, largeFiles.size());
        for (int k = 0; k < numChecks; ++k) {
            int idx = rng->bounded(largeFiles.size());
            const FileInfo& fi = largeFiles[idx];
            QString srcPath = QDir(m_source).filePath(fi.relPath);
            QString destPath = QDir(m_destination).filePath(fi.relPath);
            if (!TransferTask::spotCheckFile(srcPath, destPath, 1, 5)) {
                qWarning() << "Spot-check failed:" << fi.relPath;
                return false;
            }
        }
    }

    if (m_fullHashVerify) {
        HashManager hm;
        QString srcHash = hm.computeDirectoryHash(m_source);
        QString dstHash = hm.computeDirectoryHash(m_destination);
        if (srcHash != dstHash) {
            qWarning() << "Full recursive hash mismatch";
            return false;
        }
    }

    return true;
}

QList<TransferTask::FileInfo> TransferTask::getAllFiles(const QString& root) {
    QList<FileInfo> files;
    QDir rootDir(root);
    if (!rootDir.exists()) return files;
    QDirIterator it(root, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        QFileInfo fi(it.filePath());
        QString relPath = rootDir.relativeFilePath(fi.absoluteFilePath());
        files.append({relPath, fi.size()});
    }
    return files;
}

bool TransferTask::spotCheckFile(const QString &srcPath, const QString &dstPath, int numChunks, qint64 minSizeMB) {
    QFile srcFile(srcPath);
    QFile destFile(dstPath);
    if (!srcFile.open(QIODevice::ReadOnly) || !destFile.open(QIODevice::ReadOnly)) {
        return false;
    }
    qint64 fileSize = srcFile.size();
    if (fileSize != destFile.size()) {
        srcFile.close();
        destFile.close();
        return false;
    }
    if (fileSize < minSizeMB * 1024LL * 1024) {
        srcFile.close();
        destFile.close();
        return true;
    }
    qint64 chunkSize = fileSize / std::max(1, numChunks);
    QRandomGenerator* rng = QRandomGenerator::global();
    for (int i = 0; i < numChunks; ++i) {
        qint64 offset = rng->bounded(static_cast<quint64>(fileSize - chunkSize + 1));
        srcFile.seek(offset);
        destFile.seek(offset);
        QByteArray srcChunk = srcFile.read(chunkSize);
        QByteArray destChunk = destFile.read(chunkSize);
        if (srcChunk.size() != chunkSize || destChunk.size() != chunkSize || srcChunk.size() != destChunk.size()) {
            srcFile.close();
            destFile.close();
            return false;
        }
        if (QCryptographicHash::hash(srcChunk, QCryptographicHash::Sha256) != QCryptographicHash::hash(destChunk, QCryptographicHash::Sha256)) {
            srcFile.close();
            destFile.close();
            return false;
        }
    }
    srcFile.close();
    destFile.close();
    return true;
}

void TransferTask::setPreset(const QString& preset) {
    m_preset = preset;
    if (preset == "LTO Tape") {
        m_verifyEnabled = true;
    }
}
