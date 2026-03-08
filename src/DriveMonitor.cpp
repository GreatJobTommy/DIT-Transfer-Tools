void DriveMonitor::pollWatchdog() {
    if (m_paused) return;
    for (const QString& folder : m_watchedFolders) {
        QStringList newFiles = getMatchingFiles(folder);
        QSet<QString> currentSeen = m_seenFiles.value(folder);
        for (const QString& file : newFiles) {
            if (!currentSeen.contains(file)) {
                emit newWatchdogFile(folder, file, m_preset);
                currentSeen.insert(file);
            }
        }
        m_seenFiles[folder] = currentSeen;
    }
}

QStringList DriveMonitor::getMatchingFiles(const QString& folder) {
    QStringList files;
    QDir dir(folder);
    if (!dir.exists()) return files;
    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (const QFileInfo& info : entries) {
        if (matchesFilters(info.absoluteFilePath()) && info.size() >= m_minFileSize) {
            files << info.absoluteFilePath();
        }
    }
    return files;
}

bool DriveMonitor::matchesFilters(const QString& filePath) {
    QFileInfo info(filePath);
    QString nameLower = info.fileName().toLower();
    for (const QString& filter : m_fileFilters) {
        QRegExp rx(filter, Qt::CaseInsensitive, QRegExp::Wildcard);
        if (rx.exactMatch(nameLower)) {
            return true;
        }
    }
    return m_fileFilters.isEmpty(); // If no filters, match all
}