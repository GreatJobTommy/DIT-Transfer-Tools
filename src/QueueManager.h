/****************************************************************************
** QueueManager.h - Queue für TransferTasks (aktiv/wartend/pausiert)
** Priorität, Status-Tracking.
** Max 350 Zeilen.
****************************************************************************/

#ifndef QUEUEMANAGER_H
#define QUEUEMANAGER_H

#include &lt;QObject&gt;
#include &lt;QQueue&gt;
#include &lt;QList&gt;
#include &quot;TransferTask.h&quot;

class QueueManager : public QObject
{
    Q_OBJECT

public:
    explicit QueueManager(QObject *parent = nullptr);
    
    void addTask(const QString &amp;src, const QString &amp;dst);
    QList&lt;TransferTask*&gt; activeTasks() const;
    QList&lt;TransferTask*&gt; waitingTasks() const;
    void pauseAll();
    void resumeAll();

signals:
    void queueChanged();

private slots:
    void onTaskStatusChanged(TransferTask::Status status);

private:
    QList&lt;TransferTask*&gt; m_active;
    QQueue&lt;TransferTask*&gt; m_waiting;
    int m_maxActive = 3; // Concurrency
};

#endif // QUEUEMANAGER_H