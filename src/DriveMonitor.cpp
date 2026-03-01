#include "DriveMonitor.h"
#include <QDebug>

DriveMonitor::DriveMonitor(QObject *parent) : QObject(parent) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &DriveMonitor::checkDrives);
    m_timer->start(5000); // Check every 5 seconds
    checkDrives();
}

DriveMonitor::~DriveMonitor() {}

QList<QStorageInfo> DriveMonitor::availableDrives() const {
    return QStorageInfo::mountedVolumes();
}

void DriveMonitor::checkDrives() {
    QList<QStorageInfo> currentDrives = availableDrives();

    // Find added drives
    for (const QStorageInfo& drive : currentDrives) {
        if (!m_previousDrives.contains(drive)) {
            emit driveAdded(drive);
        }
    }

    // Find removed drives
    for (const QStorageInfo& drive : m_previousDrives) {
        if (!currentDrives.contains(drive)) {
            emit driveRemoved(drive);
        }
    }

    m_previousDrives = currentDrives;
}