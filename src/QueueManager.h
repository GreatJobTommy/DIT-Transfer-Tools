#ifndef QUEUEMANAGER_H
#define QUEUEMANAGER_H

#include <QList>
#include <QObject>
#include "TransferTask.h"
#include <QThreadPool>

class QueueManager : public QObject {
    Q_OBJECT

public:
    explicit QueueManager(int maxActive = 1, QObject* parent = nullptr);
    ~QueueManager();

    void addTask(TransferTask* task);
    void removeTask(TransferTask* task);
    void reorderWaiting(int from, int to);
    void setWaitingOrder(const QList<TransferTask*>& order);

    QList<TransferTask*> activeTasks() const;
    QList<TransferTask*> waitingTasks() const;

    int maxActive() const;
    void setMaxActive(int max);

signals:
    void taskCompleted(TransferTask* task);
    void taskFailed(TransferTask* task);
    void taskPaused(TransferTask* task);

private slots:
    void onTaskStatusChanged(TransferStatus status);

private:
    void activateNext();

    QList<TransferTask*> m_active;
    QList<TransferTask*> m_waiting;
    int m_maxActive;
    QThreadPool m_threadPool;
};

#endif // QUEUEMANAGER_H