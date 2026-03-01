#ifndef TRANSFERTASK_H
#define TRANSFERTASK_H

#include <QString>
#include <QObject>
#include <QRunnable>
#include <QFileInfo>

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

    // Other methods as needed, e.g., progress, etc.

    void run() override;

signals:
    void statusChanged(TransferStatus status);
    void progressChanged(qint64 bytes, qint64 speed, qint64 eta);

private:
    QString m_source;
    QString m_destination;
    TransferStatus m_status;
    bool m_finished;
    bool m_success;
    qint64 m_totalBytes;
    qint64 m_bytesTransferred;
};

#endif // TRANSFERTASK_H