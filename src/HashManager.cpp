#include &quot;HashManager.h&quot;
#include &lt;QDebug&gt;

QByteArray HashManager::computeHash(const QString &amp;filePath, QCryptographicHash::Algorithm algo) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() &lt;&lt; &quot;Cannot open file:&quot; &lt;&lt; filePath;
        return QByteArray();
    }
    QCryptographicHash hash(algo);
    if (!hash.addData(&amp;file)) {
        return QByteArray();
    }
    return hash.result();
}

bool HashManager::verifyHash(const QString &amp;filePath, const QByteArray &amp;expectedHash) {
    QByteArray actual = computeHash(filePath);
    return actual == expectedHash;
}
