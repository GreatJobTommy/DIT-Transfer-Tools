#include &lt;QTest&gt;
#include "../src/ErrorManager.h"

class TestErrorHandling : public QObject {
    Q_OBJECT

private slots:
    void testCategories() {
        QCOMPARE(ErrorManager::categoryToString(ErrorCategory::DiskFull), QString("Disk Full"));
    }

    void testRetryLogic() {
        ErrorManager mgr;
        // Test would need mocking for full retry
        QVERIFY(!mgr.retryLogic(3, 3));  // 4th attempt fails
    }
};

QTEST_MAIN(TestErrorHandling)
#include "test_errorhandling.moc"
