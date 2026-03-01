#include "DriveMonitor.h"
#include <QDir>
#include <QDebug>

DriveMonitor::DriveMonitor(QObject* parent)
    : QObject(parent), m_watcher(new QFileSystemWatcher(this)), m_pollTimer(new QTimer(this)) {
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &DriveMonitor::onDirectoryChanged);
    connect(m_pollTimer, &QTimer::timeout, this, &DriveMonitor::pollDrives);

    // Start polling every 5 seconds
    m_pollTimer->start(5000);

    scanDrives();
}

DriveMonitor::~DriveMonitor() {
    m_pollTimer->stop();
}

QList<QStorageInfo> DriveMonitor::getCurrentDrives() const {
    return m_currentDrives;
}

bool DriveMonitor::isRemovableDrive(const QStorageInfo& info) const {
    return info.isReady() && (info.device().startsWith("/dev/sd") || info.device().startsWith("/dev/mmc") || info.rootPath() != "/");
}

void DriveMonitor::addDrive(const QString& path) {
    if (!m_watcher->directories().contains(path)) {
        m_watcher->addPath(path);
        m_lastFiles[path] = !QDir(path).entryList(QDir::Files | QDir::NoDotAndDotDot).isEmpty();
    }
}

void DriveMonitor::removeDrive(const QString& path) {
    m_watcher->removePath(path);
    m_lastFiles.remove(path);
}

void DriveMonitor::onDirectoryChanged(const QString& path) {
    bool hasFiles = !QDir(path).entryList(QDir::Files | QDir::NoDotAndDotDot).isEmpty();
    if (hasFiles != m_lastFiles.value(path, false)) {
        if (hasFiles) {
            emit driveConnected(path);
        } else {
            emit driveDisconnected(path);
        }
        m_lastFiles[path] = hasFiles;
    }
}

void DriveMonitor::pollDrives() {
    QList<QStorageInfo> newDrives = QStorageInfo::mountedVolumes();
    QList<QString> newPaths;
    for (const QStorageInfo& info : newDrives) {
        if (info.isReady()) {
            newPaths.append(info.rootPath());
        }
    }

    QList<QString> currentPaths;
    for (const QStorageInfo& info : m_currentDrives) {
        currentPaths.append(info.rootPath());
    }

    // Check for new drives
    for (const QString& path : newPaths) {
        if (!currentPaths.contains(path)) {
            addDrive(path);
            emit driveConnected(path);
        }
    }

    // Check for removed drives
    for (const QString& path : currentPaths) {
        if (!newPaths.contains(path)) {
            removeDrive(path);
            emit driveDisconnected(path);
        }
    }

    m_currentDrives = newDrives;
}

void DriveMonitor::scanDrives() {
    m_currentDrives = QStorageInfo::mountedVolumes();
    for (const QStorageInfo& info : m_currentDrives) {
        if (info.isReady() && isRemovableDrive(info)) {
            addDrive(info.rootPath());
            emit driveConnected(info.rootPath());
        }
    }
}