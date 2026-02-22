#ifndef QUEUE_H
#define QUEUE_H
#include <QObject>
#include <QQueue>
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
    QQueue<TransferTask*> m_queue;
};
#endif
