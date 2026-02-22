#ifndef HASHMANAGER_H
#define HASHMANAGER_H
#include <QObject>
#include <QFile>
#include <QCryptographicHash>
class HashManager : public QObject {
    Q_OBJECT
public:
    static QByteArray computeHash(const QString &filePath, QCryptographicHash::Algorithm algo = QCryptographicHash::Sha256);
    static bool verifyHash(const QString &filePath, const QByteArray &expectedHash);
};
#endif // HASHMANAGER_H
