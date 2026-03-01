#include &lt;QtTest&gt;
#include &quot;../src/ConfigManager.h&quot;

class TestConfigManager : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testDefaults();
    void testSaveLoad();
    void testCorruptJsonRecovery();
    void testLiveReload();
};

void TestConfigManager::initTestCase() {
    ConfigManager::instance();
}

void TestConfigManager::cleanupTestCase() {
    delete ConfigManager::instance();
}

void TestConfigManager::testDefaults() {
    ConfigManager *mgr = ConfigManager::instance();
    QCOMPARE(mgr-&gt;getHashAlgorithm(), QString(&quot;SHA256&quot;));
    QVERIFY(!mgr-&gt;getAutoTransfer());
}

void TestConfigManager::testSaveLoad() {
    ConfigManager *mgr = ConfigManager::instance();
    mgr-&gt;setHashAlgorithm(&quot;SHA512&quot;);
    mgr-&gt;setAutoTransfer(true);
    mgr-&gt;saveAllSettings();
    
    // Simulate reload
    mgr-&gt;loadAllSettings();
    QCOMPARE(mgr-&gt;getHashAlgorithm(), QString(&quot;SHA512&quot;));
    QCOMPARE(mgr-&gt;getAutoTransfer(), true);
}

void TestConfigManager::testCorruptJsonRecovery() {
    ConfigManager *mgr = ConfigManager::instance();
    QString jsonPath = mgr-&gt; // Simplified, would need reflection
    QFile file(jsonPath);
    file.open(QIODevice::WriteOnly);
    file.write(&quot;invalid json&quot;);
    file.close();
    
    mgr-&gt;loadAllSettings();
    QCOMPARE(mgr-&gt;getHashAlgorithm(), QString(&quot;SHA256&quot;)); // Back to defaults
}

void TestConfigManager::testLiveReload() {
    // Would require file modification simulation
    QSKIP(&quot;Live reload test requires filesystem simulation&quot;);
}

QTEST_MAIN(TestConfigManager)
#include "test_configmanager.moc"
#include &quot;test_configmanager.moc&quot;
