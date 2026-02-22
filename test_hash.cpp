#include &lt;QTest&gt;
#include &lt;QDebug&gt;
#include &quot;src/HashManager.h&quot;
#include &lt;QThread&gt;

class TestHash : public QObject {
    Q_OBJECT
private slots:
    void initTestCase() { m_mgr = new HashManager(); }
    void cleanupTestCase() { delete m_mgr; }
    
    void testUpdate() {
        m_mgr-&gt;update(&quot;hello&quot;);
        QCOMPARE(m_mgr-&gt;finalHash().left(8), QString(&quot;2cf24dba&quot;));
    }
    
    void testReset() {
        m_mgr-&gt;update(&quot;world&quot;);
        m_mgr-&gt;reset();
        QCOMPARE(m_mgr-&gt;finalHash(), QString());
    }
    
private:
    HashManager *m_mgr;
};

QTEST_MAIN(TestHash)
#include &quot;test_hash.moc&quot;