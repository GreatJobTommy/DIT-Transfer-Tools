#ifndef QUEUEMANAGER_H
#define QUEUEMANAGER_H

#include <QList>
#include <QObject>
#include "TransferTask.h"

class QueueManager : public QObject {
    Q_OBJECT

public:
    explicit QueueManager(int maxActive = 1, QObject* parent = nullptr);
    ~QueueManager();

    void addTask(TransferTask* task);
    void removeTask(TransferTask* task);
    void reorderWaiting(int from, int to);

    QList<TransferTask*> activeTasks() const;
    QList<TransferTask*> waitingTasks() const;

    int maxActive() const;
    void setMaxActive(int max);

private slots:
    void onTaskStatusChanged(TransferStatus status);

private:
    void activateNext();

    QList<TransferTask*> m_active;
    QList<TransferTask*> m_waiting;
    int m_maxActive;
};

#endif // QUEUEMANAGER_H