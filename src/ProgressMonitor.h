#ifndef PROGRESSMONITOR_H
#define PROGRESSMONITOR_H

#include <QObject>
#include <QMap>
#include <QString>

class TransferTask;

class ProgressMonitor : public QObject {
    Q_OBJECT

public:
    explicit ProgressMonitor(QObject* parent = nullptr);
    ~ProgressMonitor();

    void addTask(TransferTask* task);
    void removeTask(TransferTask* task);
    QMap<TransferTask*, int> getProgress() const;
    QString getLogs() const;

signals:
    void progressUpdated(TransferTask* task, int percent);
    void taskCompleted(TransferTask* task);
    void logUpdated(const QString& log);

private:
    QMap<TransferTask*, int> m_progress;
    QString m_logs;
};

#endif // PROGRESSMONITOR_H