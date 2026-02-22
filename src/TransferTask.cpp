#include &quot;TransferTask.h&quot;
<<<<<<< HEAD
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QFileDialog>
#include <QStorageInfo>

TransferTask::TransferTask(const QString &src, const QStringList &targets, QObject *parent)
=======
#include &lt;QDebug&gt;
#include &lt;QDir&gt;
#include &lt;QFileInfo&gt;
#include &lt;QStandardPaths&gt;
#include &lt;QFileDialog&gt;
#include &lt;QStorageInfo&gt;

TransferTask::TransferTask(const QString &amp;src, const QStringList &amp;targets, QObject *parent)
>>>>>>> origin/main
    : QObject(parent), QRunnable(), m_source(src), m_targets(targets), m_progress(new ProgressTracker(this))
{
    QFileInfo fi(src);
    m_totalBytes = fi.size();
    if (m_totalBytes == 0) {
        emit errorOccurred(&quot;Source file empty or not found&quot;);
        setStatus(TaskStatus::Failed);
        return;
    }
    m_resumeStateFile = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + &quot;/&quot; + fi.fileName() + &quot;.state.json&quot;;
    m_expectedHash = HashManager::computeHash(src);
    determineChunkSize();
<<<<<<< HEAD
    connect(m_progress, &ProgressTracker::progressUpdated, this, &TransferTask::progressUpdate);
}

TransferTask::TransferTask(const QJsonObject &state, QObject *parent)
=======
    connect(m_progress, &amp;ProgressTracker::progressUpdated, this, &amp;TransferTask::progressUpdate);
}

TransferTask::TransferTask(const QJsonObject &amp;state, QObject *parent)
>>>>>>> origin/main
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
<<<<<<< HEAD
    if (size < 1*1024*1024) m_chunkSize = size;
    else if (size < 64*1024*1024) m_chunkSize = 1*1024*1024; // 1MB min
    else m_chunkSize = size / 64; // Max 64 chunks, auto-size
    if (m_chunkSize < 1024*1024) m_chunkSize = 1024*1024; // At least 1MB
    if (m_chunkSize > 64*1024*1024) m_chunkSize = 64*1024*1024;
    qDebug() << &quot;Chunk size:&quot; << m_chunkSize;
=======
    if (size &lt; 1*1024*1024) m_chunkSize = size;
    else if (size &lt; 64*1024*1024) m_chunkSize = 1*1024*1024; // 1MB min
    else m_chunkSize = size / 64; // Max 64 chunks, auto-size
    if (m_chunkSize &lt; 1024*1024) m_chunkSize = 1024*1024; // At least 1MB
    if (m_chunkSize &gt; 64*1024*1024) m_chunkSize = 64*1024*1024;
    qDebug() &lt;&lt; &quot;Chunk size:&quot; &lt;&lt; m_chunkSize;
>>>>>>> origin/main
}

void TransferTask::run() {
    if (m_status == TaskStatus::Failed) return;

    setStatus(TaskStatus::Active);
<<<<<<< HEAD
    m_progress->start();
=======
    m_progress-&gt;start();
>>>>>>> origin/main

    // Parallel multi-target using QThreadPool for chunks across targets
    QThreadPool chunkPool;
    chunkPool.setMaxThreadCount(QThread::idealThreadCount());

    struct ChunkJob : QRunnable {
        QString target;
        QString source;
        qint64 offset, size;
        QByteArray expectedHash;
<<<<<<< HEAD
        std::function<bool(const QString&, qint64, qint64)> copyFunc;
        std::function<bool(const QString&)> verifyFunc;
=======
        std::function&lt;bool(const QString&amp;, qint64, qint64)&gt; copyFunc;
        std::function&lt;bool(const QString&amp;)&gt; verifyFunc;
>>>>>>> origin/main
        bool success = true;
        void run() override {
            success = copyFunc(target, offset, size);
            if (success) success = verifyFunc(target);
        }
    };

<<<<<<< HEAD
    for (const QString &target : m_targets) {
        // Check disk space first
        QStorageInfo storage(target);
        if (storage.bytesAvailable() < m_totalBytes) {
=======
    for (const QString &amp;target : m_targets) {
        // Check disk space first
        QStorageInfo storage(target);
        if (storage.bytesAvailable() &lt; m_totalBytes) {
>>>>>>> origin/main
            emit errorOccurred(QString(&quot;Disk full on %1: %2 available&quot;).arg(target).arg(storage.bytesAvailable()));
            setStatus(TaskStatus::Failed);
            emit finished(false);
            return;
        }

        qint64 offset = 0;
<<<<<<< HEAD
        while (offset < m_totalBytes) {
            qint64 chunkSize = qMin(m_chunkSize, m_totalBytes - offset);
            ChunkJob *job = new ChunkJob();
            job->target = target;
            job->source = m_source;
            job->offset = offset;
            job->size = chunkSize;
            job->expectedHash = m_expectedHash;
            job->copyFunc = [this](const QString& tgt, qint64 off, qint64 sz) { return copyChunk(tgt, off, sz); };
            job->verifyFunc = [this](const QString& tgt) { return verifyComplete(tgt); };
            chunkPool.start(job);
            offset += chunkSize;
            // Wait for some completion to avoid OOM
            if (chunkPool.activeThreadCount() > 8) chunkPool.waitForDone(-1);
=======
        while (offset &lt; m_totalBytes) {
            qint64 chunkSize = qMin(m_chunkSize, m_totalBytes - offset);
            ChunkJob *job = new ChunkJob();
            job-&gt;target = target;
            job-&gt;source = m_source;
            job-&gt;offset = offset;
            job-&gt;size = chunkSize;
            job-&gt;expectedHash = m_expectedHash;
            job-&gt;copyFunc = [this](const QString&amp; tgt, qint64 off, qint64 sz) { return copyChunk(tgt, off, sz); };
            job-&gt;verifyFunc = [this](const QString&amp; tgt) { return verifyComplete(tgt); };
            chunkPool.start(job);
            offset += chunkSize;
            // Wait for some completion to avoid OOM
            if (chunkPool.activeThreadCount() &gt; 8) chunkPool.waitForDone(-1);
>>>>>>> origin/main
        }
    }

    chunkPool.waitForDone();

    // Final check for all targets
    bool allGood = true;
<<<<<<< HEAD
    for (const QString &target : m_targets) {
=======
    for (const QString &amp;target : m_targets) {
>>>>>>> origin/main
        if (!verifyComplete(target)) {
            allGood = false;
            emit errorOccurred(QString(&quot;Verification failed for %1&quot;).arg(target));
        }
    }

    setStatus(allGood ? TaskStatus::Completed : TaskStatus::Failed);
<<<<<<< HEAD
    m_progress->stop();
    emit finished(allGood);
}

bool TransferTask::copyChunk(const QString &target, qint64 offset, qint64 size) {
=======
    m_progress-&gt;stop();
    emit finished(allGood);
}

bool TransferTask::copyChunk(const QString &amp;target, qint64 offset, qint64 size) {
>>>>>>> origin/main
    QFile src(m_source);
    QFile dst(target);
    if (!src.open(QIODevice::ReadOnly)) {
        emit errorOccurred(QString(&quot;Cannot read %1&quot;).arg(m_source));
        return false;
    }
    if (!dst.open(QIODevice::ReadWrite)) {
        emit errorOccurred(QString(&quot;Permission denied or cannot write to %1&quot;).arg(target));
        return false;
    }
    src.seek(offset);
    QByteArray data = src.read(size);
<<<<<<< HEAD
    if (data.size() != static_cast<qint64>(size)) {
=======
    if (data.size() != static_cast&lt;qint64&gt;(size)) {
>>>>>>> origin/main
        emit errorOccurred(&quot;Read error: incomplete chunk&quot;);
        return false;
    }
    dst.seek(offset);
    qint64 written = dst.write(data);
    src.close();
    dst.close();
    if (written != size) {
        emit errorOccurred(QString(&quot;Write error: wrote %1/%2 bytes&quot;).arg(written).arg(size));
        return false;
    }
    m_transferredBytes += size; // Thread-unsafe, but approx
<<<<<<< HEAD
    m_progress->updateProgress(m_transferredBytes, m_totalBytes);
    return true;
}

bool TransferTask::verifyComplete(const QString &target) {
=======
    m_progress-&gt;updateProgress(m_transferredBytes, m_totalBytes);
    return true;
}

bool TransferTask::verifyComplete(const QString &amp;target) {
>>>>>>> origin/main
    QByteArray actual = HashManager::computeHash(target);
    bool ok = (actual == m_expectedHash);
    emit verified(ok);
    return ok;
}

QJsonObject TransferTask::toJson() const {
    QJsonObject obj;
    obj[&quot;source&quot;] = m_source;
    obj[&quot;targets&quot;] = QJsonArray::fromStringList(m_targets);
<<<<<<< HEAD
    obj[&quot;transferredBytes&quot;] = static_cast<double>(m_transferredBytes);
    obj[&quot;totalBytes&quot;] = static_cast<double>(m_totalBytes);
    obj[&quot;status&quot;] = static_cast<int>(m_status);
    return obj;
}

bool TransferTask::loadState(const QJsonObject &state) {
    m_source = state[&quot;source&quot;].toString();
    m_transferredBytes = state[&quot;transferredBytes&quot;].toDouble();
    m_totalBytes = state[&quot;totalBytes&quot;].toDouble();
    m_status = static_cast<TaskStatus>(state[&quot;status&quot;].toInt());
=======
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
>>>>>>> origin/main
    // targets etc.
    return true;
}
