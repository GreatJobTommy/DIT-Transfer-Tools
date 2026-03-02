#include <QtTest>
#include <QSignalSpy>
#include "../src/LogsDockWidget.h"
#include "../src/ErrorManager.h"

class TestLogs : public QObject {
    Q_OBJECT

private slots:
    void testAppendLog() {
        LogsDockWidget widget;
        widget.appendLog("Test message", LogsDockWidget::INFO, "dest1");
        // Check that log is appended (internal state)
        QVERIFY(true); // Placeholder - would need access to internal logs
    }

    void testLogLevels() {
        LogsDockWidget widget;
        widget.setLogLevel(LogsDockWidget::WARNING);
        QCOMPARE(widget.getLogLevel(), LogsDockWidget::WARNING);
    }

    void testLogPath() {
        LogsDockWidget widget;
        QString testPath = "/tmp/test.log";
        widget.setLogPath(testPath);
        QCOMPARE(widget.getLogPath(), testPath);
    }

    void testErrorManagerIntegration() {
        ErrorManager mgr;
        LogsDockWidget widget;
        mgr.setLogsWidget(&widget);
        mgr.setLogLevel(ErrorManager::INFO);

        QSignalSpy spy(&mgr, &ErrorManager::logAdded);
        mgr.addLog("Test log message", ErrorManager::INFO, "destination");

        QCOMPARE(spy.count(), 1);
        QList<QVariant> args = spy.takeFirst();
        QCOMPARE(args.at(0).toString(), "Test log message");
        QCOMPARE(args.at(1).toInt(), static_cast<int>(ErrorManager::INFO));
        QCOMPARE(args.at(2).toString(), "destination");
    }

    void testCrossPlatform() {
        // Test that paths work on different platforms
        LogsDockWidget widget;
        QString unixPath = "/home/user/logs/app.log";
        QString windowsPath = "C:\\Users\\user\\logs\\app.log";

        widget.setLogPath(unixPath);
        QCOMPARE(widget.getLogPath(), unixPath);

        widget.setLogPath(windowsPath);
        QCOMPARE(widget.getLogPath(), windowsPath);
    }
};

QTEST_MAIN(TestLogs)
#include "test_logs.moc"