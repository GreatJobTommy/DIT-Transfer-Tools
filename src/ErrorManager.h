#ifndef ERRORMANAGER_H
#define ERRORMANAGER_H

#include <QObject>
#include <QStringList>

class ErrorManager : public QObject {
    Q_OBJECT

public:
    explicit ErrorManager(QObject *parent = nullptr);
    ~ErrorManager();

    void logError(const QString& error);
    QStringList errors() const;

signals:
    void errorAdded(const QString& error);

private:
    QStringList m_errors;
};

#endif // ERRORMANAGER_H