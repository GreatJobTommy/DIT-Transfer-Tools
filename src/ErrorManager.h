#ifndef ERROR_MANAGER_H
#define ERROR_MANAGER_H

#include <QObject>
#include <QMessageBox>
#include <QString>

// Error categories
enum class ErrorCategory {
    DiskFull,
    TaskFail,
    DriveUnmount,
    NetworkError,
    HashMismatch,
    Unknown
};

class ErrorManager : public QObject {
    Q_OBJECT

public:
    explicit ErrorManager(QObject *parent = nullptr);

    static QString categoryToString(ErrorCategory cat);
    static ErrorCategory stringToCategory(const QString&amp; str);

    bool handleError(ErrorCategory cat, const QString&amp; message, int maxRetries = 3);
    void showUserDialog(ErrorCategory cat, const QString&amp; message);

signals:
    void errorOccurred(ErrorCategory cat, const QString&amp; message);

private:
    bool retryLogic(int attempt, int maxRetries);
    void logError(ErrorCategory cat, const QString&amp; message);
};

#endif // ERROR_MANAGER_H
