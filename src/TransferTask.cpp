#include &quot;TransferTask.h&quot;
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QFileDialog>
#include <QStorageInfo>

TransferTask::TransferTask(const QString &src, const QStringList &targets, QObject *parent)
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
    connect(m_progress, &ProgressTracker::progressUpdated, this, &TransferTask::progressUpdate);
}

TransferTask::TransferTask(const QJsonObject &state, QObject *parent)
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
    if (size < 1*1024*1024) m_chunkSize = size;
    else if (size < 64*1024*1024) m_chunkSize = 1*1024*1024; // 1MB min
    else m_chunkSize = size / 64; // Max 64 chunks, auto-size
    if (m_chunkSize < 1024*1024) m_chunkSize = 1024*1024; // At least 1MB
    if (m_chunkSize > 64*1024*1024) m_chunkSize = 64*1024*1024;
    qDebug() << &quot;Chunk size:&quot; << m_chunkSize;
}

void TransferTask::run() {
    if (m_status == TaskStatus::Failed) return;

    setStatus(TaskStatus::Active);
    m_progress->start();

    // Parallel multi-target using QThreadPool for chunks across targets
    QThreadPool chunkPool;
    chunkPool.setMaxThreadCount(QThread::idealThreadCount());

    struct ChunkJob : QRunnable {
        QString target;
        QString source;
        qint64 offset, size;
        QByteArray expectedHash;
        std::function<bool(const QString&, qint64, qint64)> copyFunc;
        std::function<bool(const QString&)> verifyFunc;
        bool success = true;
        void run() override {
            success = copyFunc(target, offset, size);
            if (success) success = verifyFunc(target);
        }
    };

    for (const QString &target : m_targets) {
        // Check disk space first
        QStorageInfo storage(target);
        if (storage.bytesAvailable() < m_totalBytes) {
            emit errorOccurred(QString(&quot;Disk full on %1: %2 available&quot;).arg(target).arg(storage.bytesAvailable()));
            setStatus(TaskStatus::Failed);
            emit finished(false);
            return;
        }

        qint64 offset = 0;
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
        }
    }

    chunkPool.waitForDone();

    // Final check for all targets
    bool allGood = true;
    for (const QString &target : m_targets) {
        if (!verifyComplete(target)) {
            allGood = false;
            emit errorOccurred(QString(&quot;Verification failed for %1&quot;).arg(target));
        }
    }

    setStatus(allGood ? TaskStatus::Completed : TaskStatus::Failed);
    m_progress->stop();
    emit finished(allGood);
}

bool TransferTask::copyChunk(const QString &target, qint64 offset, qint64 size) {
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
    if (data.size() != static_cast<qint64>(size)) {
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
    m_progress->updateProgress(m_transferredBytes, m_totalBytes);
    return true;
}

bool TransferTask::verifyComplete(const QString &target) {
    QByteArray actual = HashManager::computeHash(target);
    bool ok = (actual == m_expectedHash);
    emit verified(ok);
    return ok;
}

QJsonObject TransferTask::toJson() const {
    QJsonObject obj;
    obj[&quot;source&quot;] = m_source;
    obj[&quot;targets&quot;] = QJsonArray::fromStringList(m_targets);
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
    // targets etc.
    return true;
}
