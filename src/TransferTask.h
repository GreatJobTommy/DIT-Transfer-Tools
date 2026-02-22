#ifndef TRANSFER_TASK_H
#define TRANSFER_TASK_H

#include &lt;QObject&gt;
#include "ProgressTracker.h"

class TransferTask : public QObject {
    Q_OBJECT

public:
    explicit TransferTask(QObject *parent = nullptr);
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
