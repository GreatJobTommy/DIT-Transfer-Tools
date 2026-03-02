#ifndef TRANSFERTASK_H
#define TRANSFERTASK_H

#include <QString>
#include <QObject>
#include <QRunnable>
#include <QFileInfo>
#include <QMap>

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
    TransferTask(const QString& source, const QMap<QString, QString>& destinations, QObject* parent = nullptr);
    ~TransferTask();

    QString source() const;
    QMap<QString, QString> destinations() const;
    TransferStatus status() const;
    void setStatus(TransferStatus status);
    bool isFinished() const;
    bool success() const;
    qint64 totalBytes() const;
    qint64 bytesTransferred(const QString& dest = QString()) const;
    QMap<QString, qint64> bytesTransferredPerDest() const;

    void run() override;

signals:
    void statusChanged(TransferStatus status);
    void progressChanged(const QString& dest, qint64 bytes, qint64 speed, qint64 eta);

private:
    QString m_source;
    QMap<QString, QString> m_destinations;
    TransferStatus m_status;
    bool m_finished;
    bool m_success;
    qint64 m_totalBytes;
    QMap<QString, qint64> m_bytesTransferredPerDest;
};

#endif // TRANSFERTASK_H