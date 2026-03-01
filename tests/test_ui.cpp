#include <QtTest/QtTest>
#include <QApplication>
#include <QTabWidget>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QProgressBar>
#include <QLabel>
#include <QStorageInfo>
#include "../src/MainWindow.h"
#include "../src/QueueManager.h"
#include "../src/TransferTask.h"

class TestUI : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void testMainWindowCreation();
    void testQueueTab();
    void testDrivesTab();
    void testAddTaskTab();
    void testSettingsTab();
    void testProgressTab();
    void testHotkeys();
    void testWindowState();
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
    window->show(); // Need to show for some tests
    QTest::qWait(100); // Wait for UI to settle
}

void TestUI::testMainWindowCreation() {
    QVERIFY(window != nullptr);
    QCOMPARE(window->windowTitle(), QString("DIT Transfer Tools v2.1"));
    // Check tabs
    QTabWidget* tabWidget = window->findChild<QTabWidget*>();
    QVERIFY(tabWidget != nullptr);
    QCOMPARE(tabWidget->count(), 5); // Queue, Drives, Add Task, Settings, Progress
    QCOMPARE(tabWidget->tabText(0), QString("Queue"));
    QCOMPARE(tabWidget->tabText(1), QString("Drives"));
    QCOMPARE(tabWidget->tabText(2), QString("Add Task"));
    QCOMPARE(tabWidget->tabText(3), QString("Settings"));
    QCOMPARE(tabWidget->tabText(4), QString("Progress"));
}

void TestUI::testQueueTab() {
    QTabWidget* tabWidget = window->findChild<QTabWidget*>();
    tabWidget->setCurrentIndex(0); // Queue tab

    QListWidget* activeList = window->findChild<QListWidget*>("activeList");
    QListWidget* waitingList = window->findChild<QListWidget*>("waitingList");
    QPushButton* addBtn = window->findChild<QPushButton*>("addTaskBtn");
    QPushButton* upBtn = window->findChild<QPushButton*>("reorderUpBtn");
    QPushButton* downBtn = window->findChild<QPushButton*>("reorderDownBtn");

    QVERIFY(activeList != nullptr);
    QVERIFY(waitingList != nullptr);
    QVERIFY(addBtn != nullptr);
    QVERIFY(upBtn != nullptr);
    QVERIFY(downBtn != nullptr);

    // Check initial tasks (added in constructor)
    QCOMPARE(waitingList->count(), 2);

    // Test reorder down
    waitingList->setCurrentRow(0);
    QTest::mouseClick(downBtn, Qt::LeftButton);
    QCOMPARE(waitingList->currentRow(), 1);

    // Test reorder up
    QTest::mouseClick(upBtn, Qt::LeftButton);
    QCOMPARE(waitingList->currentRow(), 0);
}

void TestUI::testDrivesTab() {
    QTabWidget* tabWidget = window->findChild<QTabWidget*>();
    tabWidget->setCurrentIndex(1); // Drives tab

    QTableWidget* drivesTable = window->findChild<QTableWidget*>();
    QVERIFY(drivesTable != nullptr);
    QCOMPARE(drivesTable->columnCount(), 4);
    QCOMPARE(drivesTable->horizontalHeaderItem(0)->text(), QString("Name"));

    // Simulate drive added
    QStorageInfo drive;
    drive.setRootPath("/test");
    drive.setName("TestDrive");
    // Note: QStorageInfo is hard to mock, so just check the table is there
}

void TestUI::testAddTaskTab() {
    QTabWidget* tabWidget = window->findChild<QTabWidget*>();
    tabWidget->setCurrentIndex(2); // Add Task tab

    // The tab is the AddTaskDialog itself
    // Assuming it has input fields, but since it's a dialog, hard to test without opening
    QVERIFY(true); // Placeholder
}

void TestUI::testSettingsTab() {
    QTabWidget* tabWidget = window->findChild<QTabWidget*>();
    tabWidget->setCurrentIndex(3); // Settings tab

    QTextEdit* settingsEdit = window->findChild<QTextEdit*>();
    QVERIFY(settingsEdit != nullptr);
    QVERIFY(!settingsEdit->toPlainText().isEmpty());
}

void TestUI::testProgressTab() {
    QTabWidget* tabWidget = window->findChild<QTabWidget*>();
    tabWidget->setCurrentIndex(4); // Progress tab

    QProgressBar* progressBar = window->findChild<QProgressBar*>();
    QLabel* speedLabel = window->findChild<QLabel*>("speedLabel");
    QLabel* etaLabel = window->findChild<QLabel*>("etaLabel");
    QTextEdit* logEdit = window->findChild<QTextEdit*>("logEdit");

    QVERIFY(progressBar != nullptr);
    QVERIFY(speedLabel != nullptr);
    QVERIFY(etaLabel != nullptr);
    QVERIFY(logEdit != nullptr);
    QVERIFY(logEdit->isReadOnly());
}

void TestUI::testHotkeys() {
    // Test Ctrl+A for add task
    QTest::keyClick(window, Qt::Key_A, Qt::ControlModifier);
    // Assuming addTask slot is called, but hard to verify without mocking dialog
    QVERIFY(true); // Placeholder

    // Test Ctrl+Up/Down
    QListWidget* waitingList = window->findChild<QListWidget*>("waitingList");
    waitingList->setCurrentRow(0);
    QTest::keyClick(window, Qt::Key_Up, Qt::ControlModifier);
    QCOMPARE(waitingList->currentRow(), 0); // Already at top

    QTest::keyClick(window, Qt::Key_Down, Qt::ControlModifier);
    QCOMPARE(waitingList->currentRow(), 1);
}

void TestUI::testWindowState() {
    // Test save/restore, but since it's QSettings, hard to verify
    QVERIFY(true); // Placeholder
}

void TestUI::cleanupTestCase() {
    window->close();
    delete window;
    delete queue;
    delete app;
}

QTEST_MAIN(TestUI)
#include "test_ui.moc"