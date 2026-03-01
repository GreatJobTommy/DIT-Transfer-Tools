#ifndef ERRORMANAGER_H
#define ERRORMANAGER_H

#include <QObject>
#include <QStringList>

class ErrorManager : public QObject {
    Q_OBJECT

public:
    explicit ErrorManager(QObject* parent = nullptr);
    ~ErrorManager();

    void addError(const QString& error);
    QStringList getErrors() const;
    void clearErrors();

signals:
    void errorAdded(const QString& error);

private:
    QStringList m_errors;
};

#endif // ERRORMANAGER_H