#include "ErrorManager.h"
#include &lt;QDebug&gt;
#include &lt;QFile&gt;
#include &lt;QTextStream&gt;

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

ErrorCategory ErrorManager::stringToCategory(const QString&amp; str) {
    if (str == "Disk Full") return ErrorCategory::DiskFull;
    if (str == "Task Failed") return ErrorCategory::TaskFail;
    if (str == "Drive Unmounted") return ErrorCategory::DriveUnmount;
    if (str == "Network Error") return ErrorCategory::NetworkError;
    if (str == "Hash Mismatch") return ErrorCategory::HashMismatch;
    return ErrorCategory::Unknown;
}

bool ErrorManager::retryLogic(int attempt, int maxRetries) {
    if (attempt &gt;= maxRetries) return false;
    qDebug() &lt;&lt; "Retry attempt" &lt;&lt; (attempt + 1) &lt;&lt; "of" &lt;&lt; maxRetries;
    return true;
}

void ErrorManager::logError(ErrorCategory cat, const QString&amp; message) {
    QFile logFile("errors.log");
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&amp;logFile);
        out &lt;&lt; QTime::currentTime().toString() &lt;&lt; " [" &lt;&lt; categoryToString(cat) &lt;&lt; "] " &lt;&lt; message &lt;&lt; "\n";
        logFile.close();
    }
}

bool ErrorManager::handleError(ErrorCategory cat, const QString&amp; message, int maxRetries) {
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

void ErrorManager::showUserDialog(ErrorCategory cat, const QString&amp; message) {
    QString title = "Error: " + categoryToString(cat);
    QMessageBox::critical(nullptr, title, message);
}
