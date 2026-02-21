/****************************************************************************
** TransferTask.h - Chunk-basierter Transfer mit Resume und Progress
** Features: Chunk-Copy, Pause/Resume, Fehler-Handling.
** Max 300 Zeilen.
****************************************************************************/

#ifndef TRANSFERTASK_H
#define TRANSFERTASK_H

#include &lt;QObject&gt;
#include &lt;QFile&gt;
#include &lt;QString&gt;
#include &quot;HashManager.h&quot;

class TransferTask : public QObject
{
    Q_OBJECT

public:
    enum Status { Idle, Active, Paused, Error, Done };
    
    explicit TransferTask(const QString &amp;src, const QString &amp;dst, QObject *parent = nullptr);
    ~TransferTask();
    
    void start();
    void pause();
    void resume();
    Status status() const { return m_status; }
    qint64 progress() const { return m_progress; }
    QString hash() const { return m_hash; }

signals:
    void progressChanged(qint64 bytes, qint64 total);
    void statusChanged(Status status);
    void hashComputed(const QString &amp;sha256);
    void logMessage(const QString &amp;msg);

private slots:
    void computeChunkHash(const QByteArray &amp;data);

private:
    QString m_srcPath, m_dstPath;
    Status m_status = Idle;
    qint64 m_progress = 0;
    qint64 m_totalSize = 0;
    QFile m_srcFile, m_dstFile;
    HashManager *m_hashMgr;
    const qint64 CHUNK_SIZE = 64 * 1024; // 64KB
    
    bool openFiles();
    void processChunk();
};

#endif // TRANSFERTASK_H