## Live Hash während Chunk-Copy

Der HashManager berechnet SHA256-Hashes während des Chunk-Copy-Prozesses. Thread-sicher mit QMutex, Progress-Signale für Live-Updates.

Verwendung:
```cpp
HashManager mgr;
mgr.update(chunkData);
connect(&amp;mgr, &amp;HashManager::hashReady, this, [](const QByteArray&amp; h){
    qDebug() &lt;&lt; &quot;Live:&quot; &lt;&lt; h;
});
```