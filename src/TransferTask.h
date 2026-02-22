#ifndef TRANSFER_TASK_H
#define TRANSFER_TASK_H

#include &lt;QRunnable&gt;
#include &lt;QObject&gt;
#include &quot;QueueManager.h&quot;

class TransferTask : public QObject, public QRunnable {
    Q_OBJECT

public:
    explicit TransferTask(QObject *parent = nullptr);
    TaskStatus status() const { return m_status; }
    void setStatus(TaskStatus s) { m_status = s; }

protected:
    void run() override; // Implement transfer logic here

signals:
    void finished();

private:
    TaskStatus m_status = TaskStatus::Pending;
};

#endif // TRANSFER_TASK_H
