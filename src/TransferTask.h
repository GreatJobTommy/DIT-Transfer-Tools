#ifndef TRANSFER_TASK_H
#define TRANSFER_TASK_H

#include <QObject>
#include <QRunnable>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QThread>
#include <QStringList>
#include <QThreadPool>
#include &quot;ProgressTracker.h&quot;
#include &quot;HashManager.h&quot;

enum class TaskStatus { Pending, Active, Paused, Completed, Failed };

class TransferTask : public QObject, public QRunnable {
    Q_OBJECT

public:
    explicit TransferTask(const QString &src, const QStringList &targets, QObject *parent = nullptr);
    explicit TransferTask(const QJsonObject &state, QObject *parent = nullptr); // Resume constructor
    ~TransferTask();

    TaskStatus status() const { return m_status; }
    void setStatus(TaskStatus s) { m_status = s; }
    QString source() const { return m_source; }
    QStringList targets() const { return m_targets; }
    QJsonObject toJson() const;
    bool loadState(const QJsonObject &state);

protected:
    void run() override;

signals:
    void progressUpdate(double percentage, double mbPerSec, QString eta);
    void statusChanged(TaskStatus status);
    void finished(bool success);
    void verified(bool ok);
    void errorOccurred(const QString &error);

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
    bool copyChunk(const QString &target, qint64 offset, qint64 size);
    bool verifyComplete(const QString &target);
};

#endif // TRANSFER_TASK_H
