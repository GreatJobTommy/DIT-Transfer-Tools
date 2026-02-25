#ifndef TRANSFERTASK_H
#define TRANSFERTASK_H

#include <QString>
#include <QObject>

enum class TransferStatus {
    Pending,
    Active,
    Paused,
    Completed,
    Failed
};

class TransferTask : public QObject {
    Q_OBJECT

public:
    TransferTask(const QString& source, const QString& destination, QObject* parent = nullptr);
    ~TransferTask();

    QString source() const;
    QString destination() const;
    TransferStatus status() const;
    void setStatus(TransferStatus status);

    // Other methods as needed, e.g., progress, etc.

signals:
    void statusChanged(TransferStatus status);

private:
    QString m_source;
    QString m_destination;
    TransferStatus m_status;
};

#endif // TRANSFERTASK_H