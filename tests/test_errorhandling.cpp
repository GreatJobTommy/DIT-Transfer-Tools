#include <QtTest>
#include "../src/ErrorManager.h"

class TestErrorHandling : public QObject {
    Q_OBJECT

private slots:
    void testAddError() {
        ErrorManager mgr;
        mgr.addError("Test error");
        QCOMPARE(mgr.getErrors().size(), 1);
        QCOMPARE(mgr.getErrors().first(), QString("Test error"));
    }

    void testClearErrors() {
        ErrorManager mgr;
        mgr.addError("Test error");
        mgr.clearErrors();
        QVERIFY(mgr.getErrors().isEmpty());
    }
};

QTEST_MAIN(TestErrorHandling)
#include "test_errorhandling.moc"
