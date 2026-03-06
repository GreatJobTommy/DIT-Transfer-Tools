#ifndef TRANSFERTASK_H
#define TRANSFERTASK_H

#include <QString>
#include <QObject>
#include <QRunnable>
#include <QFileInfo>
#include <QProcess>
#include <QTimer>
#include <QElapsedTimer>

enum class TransferStatus {
    Pending,
    Active,
    Paused,
    Completed,
    Failed
};

class TransferTask : public QObject, public QRunnable {
    Q_OBJECT

public:
    TransferTask(const QString& source, const QString& destination, QObject* parent = nullptr);
    ~TransferTask();

    QString source() const;
    QString destination() const;
    TransferStatus status() const;
    void setStatus(TransferStatus status);
    bool isFinished() const;
    bool success() const;
    qint64 totalBytes() const;
    qint64 bytesTransferred() const;
    int progress() const;
    qint64 remainingSize() const;
    qint64 eta() const;
    QString hash() const;
    bool hashVerified() const;
    QString verifyReport() const;
    qint64 duration() const;

    bool isLTFS() const;

    qint64 bufferSize() const { return m_bufferSize; }
    void setBufferSize(qint64 size) { m_bufferSize = size; }
    bool verifyEnabled() const { return m_verifyEnabled; }
    void setVerifyEnabled(bool enabled) { m_verifyEnabled = enabled; }
    bool useWholeFileRsync() const { return m_useWholeFileRsync; }
    void setUseWholeFileRsync(bool use) { m_useWholeFileRsync = use; }
    void setPreset(const QString& preset);

    bool isRcloneRemote() const;

    void run();

signals:
    void statusChanged(TransferStatus status);
    void progressChanged(qint64 bytes, qint64 speed, qint64 eta);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void retryTransfer();
    void onReadyRead();

private:
    static qint64 parseSize(const QString &sizeStr);
    static qint64 parseEta(const QString &etaStr);
    QString m_source;
    QString m_destination;
    TransferStatus m_status;
    bool m_finished;
    bool m_success;
    qint64 m_totalBytes;
    qint64 m_bytesTransferred;

    QProcess* m_process;
    QTimer* m_retryTimer;
    int m_retryCount;
    int m_maxRetries;
    int m_backoffMs;

    qint64 m_chunkSize;
    QElapsedTimer m_speedTimer;
    qint64 m_lastBytes;
    QString m_hash;
    bool m_hashVerified;
    QElapsedTimer m_durationTimer;
    bool m_isLTFS{false};
    qint64 m_bufferSize{0};
    bool m_verifyEnabled{false};
    bool m_useWholeFileRsync{false};
    QString m_preset;
    QString m_verifyReport;

private:
    struct FileInfo {
        QString relPath;
        qint64 size;
    };
    static QList<FileInfo> getAllFiles(const QString &root);
    static bool spotCheckFile(const QString & srcPath, const QString & dstPath, int numChunks = 5, qint64 minSizeMB = 5);
    bool verifyTransfer();

};

#endif // TRANSFERTASK_H