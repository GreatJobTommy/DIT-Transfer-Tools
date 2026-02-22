#include "ErrorManager.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
ErrorManager::ErrorManager(QObject *parent) : QObject(parent) {}
QString ErrorManager::categoryToString(ErrorCategory cat) {
    switch (cat) {
    case ErrorCategory::DiskFull: return "Disk Full";
    case ErrorCategory::TaskFail: return "Task Failed";
    case ErrorCategory::DriveUnmount: return "Drive Unmounted";
    case ErrorCategory::NetworkError: return "Network Error";
    case ErrorCategory::HashMismatch: return "Hash Mismatch";
    default: return "Unknown Error";
    }
}
ErrorCategory ErrorManager::stringToCategory(const QString& str) {
    if (str == "Disk Full") return ErrorCategory::DiskFull;
    if (str == "Task Failed") return ErrorCategory::TaskFail;
    if (str == "Drive Unmounted") return ErrorCategory::DriveUnmount;
    if (str == "Network Error") return ErrorCategory::NetworkError;
    if (str == "Hash Mismatch") return ErrorCategory::HashMismatch;
    return ErrorCategory::Unknown;
}
bool ErrorManager::retryLogic(int attempt, int maxRetries) {
    if (attempt >= maxRetries) return false;
    qDebug() << "Retry attempt" << (attempt + 1) << "of" << maxRetries;
    return true;
}
void ErrorManager::logError(ErrorCategory cat, const QString& message) {
    QFile logFile("errors.log");
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        out << QTime::currentTime().toString() << " [" << categoryToString(cat) << "] " << message << "\n";
        logFile.close();
    }
}
bool ErrorManager::handleError(ErrorCategory cat, const QString& message, int maxRetries) {
    logError(cat, message);
    int attempt = 0;
    while (retryLogic(attempt, maxRetries)) {
        attempt++;
        // Simulate retry delay
        QThread::msleep(1000 * attempt);
        emit errorOccurred(cat, message);
        // Here would be actual retry logic for specific error
        if (/* retry succeeded */) {
            return true;
        }
    }
    showUserDialog(cat, message);
    return false;
}
void ErrorManager::showUserDialog(ErrorCategory cat, const QString& message) {
    QString title = "Error: " + categoryToString(cat);
    QMessageBox::critical(nullptr, title, message);
}
