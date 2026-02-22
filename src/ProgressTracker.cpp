#include "ProgressTracker.h"
#include <QDebug>
#include <QtMath>

ProgressTracker::ProgressTracker(QObject *parent)
    : QObject(parent), m_updateTimer(new QTimer(this)) {
    connect(m_updateTimer, &QTimer::timeout, this, &ProgressTracker::updateDisplay);
    m_updateTimer->setInterval(1000); // Update every second
}

ProgressTracker::~ProgressTracker() {
}

void ProgressTracker::start() {
    m_timer.start();
    m_transferredBytes = 0;
    m_isRunning = true;
    m_updateTimer->start();
    emit progressUpdated(0.0, 0.0, &quot;00:00:00&quot;);
}

void ProgressTracker::updateProgress(qint64 bytesTransferred, qint64 totalBytes) {
    m_transferredBytes = bytesTransferred;
    m_totalBytes = totalBytes;
}

void ProgressTracker::stop() {
    m_isRunning = false;
    m_updateTimer->stop();
    emit transferFinished();
}

void ProgressTracker::updateDisplay() {
    if (!m_isRunning || m_totalBytes == 0) return;

    qint64 elapsedMs = m_timer.elapsed();
    double elapsedSec = elapsedMs / 1000.0;
    double mbTransferred = m_transferredBytes / (1024.0 * 1024.0);
    m_mbPerSec = elapsedSec > 0 ? mbTransferred / elapsedSec : 0.0;

    double percentage = (m_transferredBytes * 100.0) / m_totalBytes;

    QString eta;
    if (m_mbPerSec > 0) {
        double remainingMb = (m_totalBytes - m_transferredBytes) / (1024.0 * 1024.0);
        double remainingSec = remainingMb / m_mbPerSec;
        qint64 hours = remainingSec / 3600;
        qint64 mins = (remainingSec / 60) % 60;
        qint64 secs = remainingSec % 60;
        eta = QString(&quot;%1:%2:%3&quot;).arg(hours).arg(mins, 2, 10, QChar(&#x27;0&#x27;)).arg(secs, 2, 10, QChar(&#x27;0&#x27;));
    } else {
        eta = &quot;--:--:--&quot;;
    }

    emit progressUpdated(percentage, m_mbPerSec, eta);
}
