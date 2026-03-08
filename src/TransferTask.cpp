QString TransferTask::getSimpleReportJSON() const {
    QJsonObject obj;
    obj[\"source\"] = m_source;
    obj[\"destination\"] = m_destination;
    obj[\"status\"] = static_cast<int>(m_status);
    obj[\"totalBytes\"] = static_cast<double>(m_totalBytes);
    obj[\"bytesTransferred\"] = static_cast<double>(m_bytesTransferred);
    obj[\"avgSpeedBps\"] = static_cast<double>(m_avgSpeed);
    obj[\"durationMs\"] = m_durationTimer.elapsed();
    obj[\"durationSec\"] = duration();
    obj[\"errorCount\"] = m_errorCount;
    obj[\"fileCount\"] = m_fileCount;
    obj[\"hashVerified\"] = m_hashVerified;
    obj[\"progressPct\"] = progress();
    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Compact);
}

void TransferTask::exportReport(const QString&amp; path) {
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(getSimpleReportJSON().toUtf8());
        file.close();
    }
}