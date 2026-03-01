#ifndef HASHMANAGER_H
#define HASHMANAGER_H

#include <QObject>
#include <QCryptographicHash>
#include <QFile>
#include <QThread>

class HashManager : public QObject {
    Q_OBJECT

public:
    explicit HashManager(QObject *parent = nullptr);
    ~HashManager();

    void computeHash(const QString& filePath);
    QString hash() const;

signals:
    void hashComputed(const QString& hash);
    void errorOccurred(const QString& error);

private:
    QCryptographicHash m_hash;
    QString m_resultHash;
    QThread* m_thread;

private slots:
    void compute();
};

#endif // HASHMANAGER_H