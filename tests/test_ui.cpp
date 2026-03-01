#include <QtTest/QtTest>
#include <QApplication>
#include "../src/MainWindow.h"
#include "../src/QueueManager.h"

class TestUI : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void testMainWindowCreation();
    void testAddTaskDialog();
    void cleanupTestCase();

private:
    QApplication* app;
    MainWindow* window;
    QueueManager* queue;
};

void TestUI::initTestCase() {
    int argc = 1;
    char* argv[] = {"test"};
    app = new QApplication(argc, argv);
    queue = new QueueManager;
    window = new MainWindow(queue);
}

void TestUI::testMainWindowCreation() {
    QVERIFY(window != nullptr);
    QCOMPARE(window->windowTitle(), QString("DIT Transfer Tools v2.1"));
    // Check tabs
    QVERIFY(window->findChild<QTabWidget*>() != nullptr);
}

void TestUI::testAddTaskDialog() {
    // Simulate adding a task
    TransferTask* task = new TransferTask("test_src", "test_dst");
    queue->addTask(task);
    QCOMPARE(queue->waitingTasks().size(), 1);
}

void TestUI::cleanupTestCase() {
    delete window;
    delete queue;
    delete app;
}

QTEST_MAIN(TestUI)
#include "test_ui.moc"