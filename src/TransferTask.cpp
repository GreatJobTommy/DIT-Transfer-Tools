#include &quot;TransferTask.h&quot;
#include &lt;QDebug&gt;
#include &lt;QDir&gt;
#include &lt;QFileInfo&gt;
#include &lt;QStandardPaths&gt;

TransferTask::TransferTask(const QString &amp;src, const QStringList &amp;targets, QObject *parent)
    : QObject(parent), QRunnable(), m_source(src), m_targets(targets), m_progress(new ProgressTracker(this))
{
    QFileInfo fi(src);
    m_totalBytes = fi.size();
    m_resumeStateFile = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + &quot;/&quot; + fi.fileName() + &quot;.state.json&quot;;
    m_expectedHash = HashManager::computeHash(src);
    determineChunkSize();
    connect(m_progress, &amp;ProgressTracker::progressUpdated, this, &amp;TransferTask::progressUpdate);
}

TransferTask::TransferTask(const QJsonObject &amp;state, QObject *parent)
    : QObject(parent), QRunnable(), m_progress(new ProgressTracker(this))
{
    loadState(state);
}

TransferTask::~TransferTask() {
    // Save state on destruction if incomplete
    if (m_status != TaskStatus::Completed) {
        QJsonObject state = toJson();
        QFile file(m_resumeStateFile);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(state).toJson());
        }
    }
}

void TransferTask::determineChunkSize() {
    qint64 size = m_totalBytes;
    if (size &lt; 1*1024*1024) m_chunkSize = size;
    else if (size &lt; 64*1024*1024) m_chunkSize = 1*1024*1024; // 1MB min
    else m_chunkSize = size / 64; // Max 64 chunks, auto-size
    if (m_chunkSize &lt; 1024*1024) m_chunkSize = 1024*1024; // At least 1MB
    if (m_chunkSize &gt; 64*1024*1024) m_chunkSize = 64*1024*1024;
    qDebug() &lt;&lt; &quot;Chunk size:&quot; &lt;&lt; m_chunkSize;
}

void TransferTask::run() {
    setStatus(TaskStatus::Active);
    m_progress-&gt;start();

    for (const QString &amp;target : m_targets) {
        qint64 offset = 0;
        while (offset &lt; m_totalBytes) {
            qint64 chunk = qMin(m_chunkSize, m_totalBytes - offset);
            if (!copyChunk(target, offset, chunk)) {
                setStatus(TaskStatus::Failed);
                emit finished(false);
                return;
            }
            offset += chunk;
            m_transferredBytes += chunk;
            m_progress-&gt;updateProgress(m_transferredBytes, m_totalBytes);
        }
        if (!verifyComplete(target)) {
            setStatus(TaskStatus::Failed);
            emit finished(false);
            return;
        }
    }

    setStatus(TaskStatus::Completed);
    m_progress-&gt;stop();
    emit finished(true);
}

bool TransferTask::copyChunk(const QString &amp;target, qint64 offset, qint64 size) {
    QFile src(m_source);
    QFile dst(target);
    if (!src.open(QIODevice::ReadOnly) || !dst.open(QIODevice::ReadWrite | QIODevice::Append)) return false;
    src.seek(offset);
    QByteArray data = src.read(size);
    dst.seek(offset); // Resume support
    qint64 written = dst.write(data);
    src.close();
    dst.close();
    return written == size;
}

bool TransferTask::verifyComplete(const QString &amp;target) {
    QByteArray actual = HashManager::computeHash(target);
    bool ok = (actual == m_expectedHash);
    emit verified(ok);
    return ok;
}

QJsonObject TransferTask::toJson() const {
    QJsonObject obj;
    obj[&quot;source&quot;] = m_source;
    obj[&quot;targets&quot;] = QJsonArray::fromStringList(m_targets);
    obj[&quot;transferredBytes&quot;] = static_cast&lt;double&gt;(m_transferredBytes);
    obj[&quot;totalBytes&quot;] = static_cast&lt;double&gt;(m_totalBytes);
    obj[&quot;status&quot;] = static_cast&lt;int&gt;(m_status);
    return obj;
}

bool TransferTask::loadState(const QJsonObject &amp;state) {
    m_source = state[&quot;source&quot;].toString();
    m_transferredBytes = state[&quot;transferredBytes&quot;].toDouble();
    m_totalBytes = state[&quot;totalBytes&quot;].toDouble();
    m_status = static_cast&lt;TaskStatus&gt;(state[&quot;status&quot;].toInt());
    // targets etc.
    return true;
}
