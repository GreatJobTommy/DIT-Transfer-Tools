#ifndef HASHMANAGER_H
#define HASHMANAGER_H

#include &lt;QObject&gt;
#include &lt;QFile&gt;
#include &lt;QCryptographicHash&gt;

class HashManager : public QObject {
    Q_OBJECT

public:
    static QByteArray computeHash(const QString &amp;filePath, QCryptographicHash::Algorithm algo = QCryptographicHash::Sha256);
    static bool verifyHash(const QString &amp;filePath, const QByteArray &amp;expectedHash);
};

#endif // HASHMANAGER_H
