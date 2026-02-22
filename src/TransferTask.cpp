#include &quot;TransferTask.h&quot;
#include &quot;HashManager.h&quot;
#include &lt;QDebug&gt;
#include &lt;QDir&gt;

TransferTask::TransferTask(QObject *parent)
    : QObject(parent), QRunnable()
{
}

void TransferTask::run() {
    // Simulated transfer logic with hash integration
    qDebug() &lt;&lt; &quot;TransferTask running...&quot;;
    
    // Example: compute hash before/after transfer
    QString src = &quot;/path/to/src&quot;;
    QString dst = &quot;/path/to/dst&quot;;
    QByteArray hash = HashManager::computeHash(src);
    // Copy file...
    QFile::copy(src, dst);
    
    if (HashManager::verifyHash(dst, hash)) {
        qDebug() &lt;&lt; &quot;Transfer verified OK&quot;;
    }
    
    setStatus(TaskStatus::Completed);
    emit finished();
}
