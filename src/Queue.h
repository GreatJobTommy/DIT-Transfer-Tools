#ifndef QUEUE_H
#define QUEUE_H

#include &lt;QObject&gt;
#include &lt;QQueue&gt;
#include "TransferTask.h"

class TransferQueue : public QObject {
    Q_OBJECT

public:
    explicit TransferQueue(QObject *parent = nullptr);
    void enqueue(TransferTask *task);
    void startNext();

signals:
    void queueProgress(double percentage);

private:
    QQueue&lt;TransferTask*&gt; m_queue;
};

#endif
