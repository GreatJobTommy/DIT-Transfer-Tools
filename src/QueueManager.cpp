/****************************************************************************
** QueueManager.cpp - Queue Logic
****************************************************************************/

#include &quot;QueueManager.h&quot;
#include &lt;QTimer&gt;

QueueManager::QueueManager(QObject *parent)
    : QObject(parent)
{
}

void QueueManager::addTask(const QString &amp;src, const QString &amp;dst)
{
    TransferTask *task = new TransferTask(src, dst, this);
    connect(task, &amp;TransferTask::statusChanged, this, &amp;QueueManager::onTaskStatusChanged);
    
    m_waiting.enqueue(task);
    emit queueChanged();
    
    // Auto-start wenn Platz
    if (m_active.size() &lt; m_maxActive) {
        TransferTask *next = m_waiting.dequeue();
        m_active.append(next);
        next-&gt;start();
    }
}

QList&lt;TransferTask*&gt; QueueManager::activeTasks() const { return m_active; }
QList&lt;TransferTask*&gt; QueueManager::waitingTasks() const { return m_waiting.toList(); }

void QueueManager::pauseAll()
{
    for (auto task : m_active) task-&gt;pause();
}

void QueueManager::resumeAll()
{
    for (auto task : m_active) task-&gt;resume();
}

void QueueManager::onTaskStatusChanged(TransferTask::Status status)
{
    TransferTask *task = qobject_cast&lt;TransferTask*&gt;(sender());
    if (!task) return;
    
    if (status == TransferTask::Done || status == TransferTask::Error) {
        m_active.removeOne(task);
        // Nächsten starten
        if (!m_waiting.isEmpty()) {
            TransferTask *next = m_waiting.dequeue();
            m_active.append(next);
            next-&gt;start();
        }
    }
    emit queueChanged();
}

// EOF