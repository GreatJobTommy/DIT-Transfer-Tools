#ifndef TRANSFER_TASK_H
#define TRANSFER_TASK_H

#include &lt;QRunnable&gt;
#include &lt;QObject&gt;
<<<<<<< HEAD
#include "ProgressTracker.h"

class TransferTask : public QObject {
=======
#include &quot;QueueManager.h&quot;

class TransferTask : public QObject, public QRunnable {
>>>>>>> origin/main
    Q_OBJECT

public:
    explicit TransferTask(QObject *parent = nullptr);
<<<<<<< HEAD
    void setTotalBytes(qint64 total);
    void addTransferredBytes(qint64 bytes);

signals:
    void progressUpdate(qint64 bytesTransferred, qint64 totalBytes);
    void finished();

private:
    ProgressTracker *m_tracker;
    qint64 m_totalBytes;
    qint64 m_transferredBytes;
};

#endif
=======
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
>>>>>>> origin/main
