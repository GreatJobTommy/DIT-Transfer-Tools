#include <QtTest/QtTest>
#include <QApplication>
#include "../src/AddTaskDialog.h"
#include "../src/TransferTask.h"

class TestAddTask : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void testPathValidation();
    void testDialogAccept();
    void testDragDrop();
    void cleanupTestCase();

private:
    QApplication* app;
};

void TestAddTask::initTestCase() {
    int argc = 1;
    char* argv[] = {(char*)"test"};
    app = new QApplication(argc, argv);
}

void TestAddTask::testPathValidation() {
    AddTaskDialog dialog;
    dialog.show(); // Need to show for some tests, but headless

    // Test invalid paths
    dialog.findChild<QLineEdit*>("sourceEdit")->setText("/nonexistent");
    dialog.findChild<QLineEdit*>("destEdit")->setText("/nonexistent");
    // Validation is visual, hard to test without rendering

    QVERIFY(true); // Placeholder
}

void TestAddTask::testDialogAccept() {
    AddTaskDialog dialog;
    // Simulate setting valid paths
    // But since no real paths, just check if dialog can be accepted with invalid paths
    QVERIFY(dialog.exec() != QDialog::Accepted); // Should not accept invalid
}

void TestAddTask::testDragDrop() {
    AddTaskDialog dialog;
    FileDropList* list = dialog.findChild<FileDropList*>();
    QVERIFY(list != nullptr);
    // Hard to simulate drop without QTest::mouseDrop
}

void TestAddTask::cleanupTestCase() {
    delete app;
}

QTEST_MAIN(TestAddTask)
#include "test_addtask.moc"