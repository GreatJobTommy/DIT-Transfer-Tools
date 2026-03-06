#include &quot;HashManager.h&quot;
#include &lt;QFile&gt;
#include &lt;QDebug&gt;
#include &lt;QDirIterator&gt;
#include &lt;QList&gt;
#include &lt;QPair&gt;
#include &lt;algorithm&gt;

HashManager::HashManager() {}

QString HashManager::computeSHA256(const QString&amp; filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() &lt;&lt; &quot;Failed to open file:&quot; &lt;&lt; filePath;
        return QString();
    }
    QCryptographicHash hash(QCryptographicHash::Sha256);
    if (hash.addData(&amp;file)) {
        return hash.result().toHex();
    }
    return QString();
}

QString HashManager::computeDirectoryHash(const QString&amp; dirPath) {
    QFileInfo dirInfo(dirPath);
    if (!dirInfo.isDir()) {
        qDebug() &lt;&lt; &quot;Not a directory:&quot; &lt;&lt; dirPath;
        return QString();
    }

    QList&lt;QPair&lt;QString, QString&gt;&gt; fileHashes;
    QDirIterator it(dirPath, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString absPath = it.next();
        QFileInfo fi(absPath);
        if (!fi.isFile()) continue;
        QString relPath = QDir(dirPath).relativeFilePath(absPath);
        QString fileHash = computeSHA256(absPath);
        if (!fileHash.isEmpty()) {
            fileHashes.append(qMakePair(relPath, fileHash));
        }
    }

    std::sort(fileHashes.begin(), fileHashes.end(), [](const QPair&lt;QString, QString&gt;&amp; a, const QPair&lt;QString, QString&gt;&amp; b) {
        return a.first &lt; b.first;
    });

    QCryptographicHash manifest(QCryptographicHash::Sha256);
    for (const QPair&lt;QString, QString&gt;&amp; fh : fileHashes) {
        manifest.addData(fh.first.toUtf8() + &quot;:&quot; + fh.second.toUtf8() + &quot;\n&quot;);
    }
    return manifest.result().toHex();
}