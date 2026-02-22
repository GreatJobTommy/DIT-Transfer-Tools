#ifndef TRANSFER_TASK_H
#define TRANSFER_TASK_H

<<<<<<< HEAD
#include <QObject>
#include <QRunnable>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QThread>
#include <QStringList>
#include <QThreadPool>
=======
#include <QObject>
#include <QRunnable>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QThread>
#include <QStringList>
#include <QThreadPool>
>>>>>>> origin/main
#include "ProgressTracker.h"
#include "HashManager.h"

enum class TaskStatus { Pending, Active, Paused, Completed, Failed };

class TransferTask : public QObject, public QRunnable {
    Q_OBJECT

public:
<<<<<<< HEAD
    explicit TransferTask(const QString &src, const QStringList &targets, QObject *parent = nullptr);
    explicit TransferTask(const QJsonObject &state, QObject *parent = nullptr); // Resume constructor
=======
    explicit TransferTask(const QString &amp;src, const QStringList &amp;targets, QObject *parent = nullptr);
    explicit TransferTask(const QJsonObject &amp;state, QObject *parent = nullptr); // Resume constructor
>>>>>>> origin/main
    ~TransferTask();

    TaskStatus status() const { return m_status; }
    void setStatus(TaskStatus s) { m_status = s; }
    QString source() const { return m_source; }
    QStringList targets() const { return m_targets; }
    QJsonObject toJson() const;
<<<<<<< HEAD
    bool loadState(const QJsonObject &state);
=======
    bool loadState(const QJsonObject &amp;state);
>>>>>>> origin/main

protected:
    void run() override;

signals:
    void progressUpdate(double percentage, double mbPerSec, QString eta);
    void statusChanged(TaskStatus status);
    void finished(bool success);
    void verified(bool ok);
<<<<<<< HEAD
    void errorOccurred(const QString &error);
=======
    void errorOccurred(ErrorCategory cat, const QString &amp;error);
>>>>>>> origin/main

private:
    QString m_source;
    QStringList m_targets;
    TaskStatus m_status = TaskStatus::Pending;
    qint64 m_totalBytes = 0;
    qint64 m_transferredBytes = 0;
    qint64 m_chunkSize = 0;
    QString m_resumeStateFile;
    ProgressTracker *m_progress;
    QByteArray m_expectedHash;

    void determineChunkSize();
<<<<<<< HEAD
    bool copyChunk(const QString &target, qint64 offset, qint64 size);
    bool verifyComplete(const QString &target);
=======
    bool copyChunk(const QString &amp;target, qint64 offset, qint64 size);
    bool verifyComplete(const QString &amp;target);
>>>>>>> origin/main
};

#endif // TRANSFER_TASK_H
