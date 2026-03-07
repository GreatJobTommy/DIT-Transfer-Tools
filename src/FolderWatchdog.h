#ifndef FOLDERWATCHDOG_H
#define FOLDERWATCHDOG_H

#include &lt;QObject&gt;
#include &lt;QFileSystemWatcher&gt;
#include &lt;QTimer&gt;
#include &lt;QMap&gt;
#include &lt;QSet&gt;
#include &lt;QStringList&gt;
#include "QueueManager.h"
#include "SettingsManager.h"
#include "TransferTask.h"

class FolderWatchdog : public QObject {
    Q_OBJECT

public:
    explicit FolderWatchdog(QueueManager* queue, SettingsManager* settings, QObject *parent = nullptr);
    ~FolderWatchdog();

    void pause();
    void resume();
    void reloadConfig();

private slots:
    void onDirectoryChanged(const QString &amp;path);
    void onPollTimeout();

private:
    void scanFolder(const QString &amp;folder);
    bool fileMatchesFilter(const QFileInfo &amp;fi);
    TransferTask* createAutoTask(const QString &amp;source);

    QFileSystemWatcher *m_watcher;
    QTimer *m_pollTimer;
    QueueManager *m_queue;
    SettingsManager *m_settings;
    QStringList m_folders;
    int m_interval;
    QString m_extensions;
    double m_minSizeGB;
    QString m_dest;
    QString m_preset;
    QMap&lt;QString, QSet&lt;QString&gt;&gt; m_seenFiles;
};

#endif // FOLDERWATCHDOG_H
