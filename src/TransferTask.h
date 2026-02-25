#ifndef TRANSFERTASK_H
#define TRANSFERTASK_H

#include <QString>
#include <QObject>
#include <QRunnable>

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

    // Other methods as needed, e.g., progress, etc.

    void run() override;

signals:
    void statusChanged(TransferStatus status);

private:
    QString m_source;
    QString m_destination;
    TransferStatus m_status;
    bool m_finished;
    bool m_success;
};

#endif // TRANSFERTASK_H