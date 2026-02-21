/****************************************************************************
** TransferTask.cpp - Implementation Chunk-Copy + Live-Hashing
****************************************************************************/

#include &quot;TransferTask.h&quot;
#include &lt;QDebug&gt;
#include &lt;QFileInfo&gt;
#include &lt;QDir&gt;

TransferTask::TransferTask(const QString &amp;src, const QString &amp;dst, QObject *parent)
    : QObject(parent), m_srcPath(src), m_dstPath(dst), m_hashMgr(new HashManager(this))
{
    connect(m_hashMgr, &amp;HashManager::hashReady, this, &amp;TransferTask::computeChunkHash);
}

TransferTask::~TransferTask()
{
    m_srcFile.close();
    m_dstFile.close();
}

void TransferTask::start()
{
    if (!openFiles()) {
        emit statusChanged(Error);
        emit logMessage(&quot;Fehler beim Öffnen der Dateien&quot;);
        return;
    }
    
    m_status = Active;
    emit statusChanged(Active);
    processChunk();
}

bool TransferTask::openFiles()
{
    QFileInfo srcInfo(m_srcPath);
    m_totalSize = srcInfo.size();
    
    if (!m_srcFile.open(QIODevice::ReadOnly)) {
        emit logMessage(&quot;Kann Quelle nicht öffnen: &quot; + m_srcPath);
        return false;
    }
    
    QDir().mkpath(QFileInfo(m_dstPath).absolutePath());
    if (!m_dstFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        emit logMessage(&quot;Kann Ziel nicht öffnen: &quot; + m_dstPath);
        return false;
    }
    
    // Resume: Position setzen
    m_dstFile.seek(m_srcFile.size()); // Vereinfacht
    return true;
}

void TransferTask::processChunk()
{
    if (m_status != Active) return;
    
    QByteArray chunk = m_srcFile.read(CHUNK_SIZE);
    if (chunk.isEmpty()) {
        m_status = Done;
        emit statusChanged(Done);
        emit hashComputed(m_hashMgr-&gt;finalHash());
        return;
    }
    
    m_hashMgr-&gt;update(chunk);
    qint64 written = m_dstFile.write(chunk);
    if (written != chunk.size()) {
        m_status = Error;
        emit statusChanged(Error);
        return;
    }
    
    m_progress += written;
    emit progressChanged(m_progress, m_totalSize);
    
    QTimer::singleShot(0, this, &amp;TransferTask::processChunk); // Async
}

void TransferTask::pause()
{
    m_status = Paused;
    emit statusChanged(Paused);
}

void TransferTask::resume()
{
    m_status = Active;
    emit statusChanged(Active);
    processChunk();
}

void TransferTask::computeChunkHash(const QByteArray &amp;data)
{
    emit logMessage(&quot;Chunk Hash: &quot; + QString::fromLatin1(data.toHex()));
}

// EOF