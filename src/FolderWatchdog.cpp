#include "FolderWatchdog.h"
#include &lt;QDir&gt;
#include &lt;QDebug&gt;
#include &lt;QFileInfo&gt;
#include &lt;QDateTime&gt;

FolderWatchdog::FolderWatchdog(QueueManager* queue, SettingsManager* settings, QObject *parent)
    : QObject(parent), m_watcher(new QFileSystemWatcher(this)), m_pollTimer(new QTimer(this)),
      m_queue(queue), m_settings(settings), m_interval(5000), m_minSizeGB(1.0) {
    connect(m_watcher, &amp;QFileSystemWatcher::directoryChanged, this, &amp;FolderWatchdog::onDirectoryChanged);
    connect(m_pollTimer, &amp;QTimer::timeout, this, &amp;FolderWatchdog::onPollTimeout);
    reloadConfig();
    resume();
}

FolderWatchdog::~FolderWatchdog() {
    pause();
}

void FolderWatchdog::pause() {
    m_pollTimer-&gt;stop();
    m_watcher-&gt;removePaths(m_watcher-&gt;directories());
}

void FolderWatchdog::resume() {
    m_watcher-&gt;removePaths(m_watcher-&gt;directories()); // Clear and re-add
    reloadConfig();
    m_pollTimer-&gt;start(m_interval);
}

void FolderWatchdog::reloadConfig() {
    m_folders = m_settings-&gt;getWatchFolders();
    m_interval = m_settings-&gt;getWatchInterval();
    m_extensions = m_settings-&gt;getWatchExtensions();
    m_minSizeGB = m_settings-&gt;getMinSizeGB();
    m_dest = m_settings-&gt;getWatchDest();
    m_preset = m_settings-&gt;getWatchPreset();

    // Re-add folders to watcher
    for (const QString&amp; folder : m_folders) {
        if (QDir(folder).exists()) {
            m_watcher-&gt;addPath(folder);
            scanFolder(folder); // Initial scan
        }
    }
}

void FolderWatchdog::onDirectoryChanged(const QString &amp;path) {
    scanFolder(path);
}

void FolderWatchdog::onPollTimeout() {
    for (const QString&amp; folder : m_folders) {
        scanFolder(folder);
    }
}

void FolderWatchdog::scanFolder(const QString &amp;folder) {
    QDir dir(folder);
    if (!dir.exists()) return;

    QStringList filters = m_extensions.split(&quot;,&quot;, Qt::SkipEmptyParts);
    QDirIterator it(folder, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo fi(it.next());
        if (fileMatchesFilter(fi)) {
            QString relPath = dir.relativeFilePath(fi.absoluteFilePath());
            if (!m_seenFiles[folder].contains(relPath)) {
                TransferTask* task = createAutoTask(fi.absoluteFilePath());
                if (task) {
                    m_queue-&gt;addTask(task);
                    m_seenFiles[folder].insert(relPath);
                    qDebug() &lt;&lt; &quot;Auto-added task for:&quot; &lt;&lt; fi.absoluteFilePath();
                }
            }
        }
    }
}

bool FolderWatchdog::fileMatchesFilter(const QFileInfo &amp;fi) {
    if (m_extensions.isEmpty()) return true;
    QString ext = fi.suffix().toLower();
    QStringList exts = m_extensions.split(&quot;,&quot;, Qt::SkipEmptyParts);
    if (!exts.contains(ext)) return false;

    qint64 sizeGB = fi.size() / (1024.0 * 1024 * 1024);
    return sizeGB &gt;= m_minSizeGB;
}

TransferTask* FolderWatchdog::createAutoTask(const QString &amp;source) {
    TransferTask* task = new TransferTask(source, m_dest);
    task-&gt;setPreset(m_preset);
    return task;
}
