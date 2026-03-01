#include "MainWindow.h"
#include "TransferTask.h"
#include <QLabel>
#include <QSplitter>
#include <QHeaderView>
#include <QCloseEvent>
#include <QShortcut>
#include <QApplication>

MainWindow::MainWindow(QueueManager* queue, QWidget* parent)
    : QMainWindow(parent), m_queue(queue) {
    setWindowTitle("DIT Transfer Tools v2.1");

    m_parallel = new ParallelManager(this);
    m_driveMonitor = new DriveMonitor(this);
    m_settings = new SettingsManager(this);
    m_progress = new ProgressMonitor(this);
    m_errors = new ErrorManager(this);
    m_hash = new HashManager(this);

    setupUI();
    setupTabs();
    restoreWindowState();

    // Connect signals
    connect(m_driveMonitor, &DriveMonitor::driveAdded, this, &MainWindow::onDriveAdded);
    connect(m_driveMonitor, &DriveMonitor::driveRemoved, this, &MainWindow::onDriveRemoved);
    connect(m_progress, &ProgressMonitor::progressUpdated, this, &MainWindow::onProgressUpdated);
    connect(m_errors, &ErrorManager::errorAdded, this, &MainWindow::onErrorAdded);

    // Hotkeys
    new QShortcut(QKeySequence("Ctrl+A"), this, SLOT(addTask()));
    new QShortcut(QKeySequence("Ctrl+Up"), this, SLOT(reorderUp()));
    new QShortcut(QKeySequence("Ctrl+Down"), this, SLOT(reorderDown()));

    // Tooltips
    m_addTaskBtn->setToolTip("Add a new transfer task (Ctrl+A)");
    m_reorderUpBtn->setToolTip("Move selected task up (Ctrl+Up)");
    m_reorderDownBtn->setToolTip("Move selected task down (Ctrl+Down)");

    // For demo, add some tasks
    TransferTask* t1 = new TransferTask("src1", "dst1");
    TransferTask* t2 = new TransferTask("src2", "dst2");
    m_queue->addTask(t1);
    m_queue->addTask(t2);

    updateLists();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    m_tabWidget = new QTabWidget;
    setCentralWidget(m_tabWidget);

    m_errorDock = new QDockWidget("Errors");
    m_errorEdit = new QTextEdit;
    m_errorEdit->setReadOnly(true);
    m_errorDock->setWidget(m_errorEdit);
    addDockWidget(Qt::BottomDockWidgetArea, m_errorDock);
}

void MainWindow::setupTabs() {
    setupQueueTab();
    setupDrivesTab();
    setupAddTaskTab();
    setupSettingsTab();
    setupProgressTab();
}

void MainWindow::setupQueueTab() {
    QWidget* queueWidget = new QWidget;
    QHBoxLayout* mainLayout = new QHBoxLayout(queueWidget);

    // Active list
    QVBoxLayout* activeLayout = new QVBoxLayout;
    activeLayout->addWidget(new QLabel("Active Transfers"));
    m_activeList = new QListWidget;
    m_activeList->setObjectName("activeList");
    activeLayout->addWidget(m_activeList);

    // Waiting list
    QVBoxLayout* waitingLayout = new QVBoxLayout;
    waitingLayout->addWidget(new QLabel("Waiting Transfers"));
    m_waitingList = new QListWidget;
    m_waitingList->setObjectName("waitingList");
    waitingLayout->addWidget(m_waitingList);

    // Buttons
    QVBoxLayout* buttonLayout = new QVBoxLayout;
    m_addTaskBtn = new QPushButton("Add Task");
    m_addTaskBtn->setObjectName("addTaskBtn");
    m_reorderUpBtn = new QPushButton("Move Up");
    m_reorderUpBtn->setObjectName("reorderUpBtn");
    m_reorderDownBtn = new QPushButton("Move Down");
    m_reorderDownBtn->setObjectName("reorderDownBtn");
    buttonLayout->addWidget(m_addTaskBtn);
    buttonLayout->addWidget(m_reorderUpBtn);
    buttonLayout->addWidget(m_reorderDownBtn);
    buttonLayout->addStretch();

    waitingLayout->addLayout(buttonLayout);

    mainLayout->addLayout(activeLayout);
    mainLayout->addLayout(waitingLayout);

    connect(m_addTaskBtn, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(m_reorderUpBtn, &QPushButton::clicked, this, &MainWindow::reorderUp);
    connect(m_reorderDownBtn, &QPushButton::clicked, this, &MainWindow::reorderDown);

    m_tabWidget->addTab(queueWidget, "Queue");
}

void MainWindow::setupDrivesTab() {
    QWidget* drivesWidget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(drivesWidget);

    m_drivesTable = new QTableWidget;
    m_drivesTable->setColumnCount(4);
    m_drivesTable->setHorizontalHeaderLabels({"Name", "Path", "Size", "Available"});
    layout->addWidget(m_drivesTable);

    m_tabWidget->addTab(drivesWidget, "Drives");
}

void MainWindow::setupAddTaskTab() {
    m_addTaskDialog = new AddTaskDialog;
    m_tabWidget->addTab(m_addTaskDialog, "Add Task");
}

void MainWindow::setupSettingsTab() {
    QWidget* settingsWidget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(settingsWidget);

    m_settingsEdit = new QTextEdit;
    m_settingsEdit->setPlainText("Settings loaded from config.ini\nMaxThreads=4\nChunkSize=8192\n...");
    layout->addWidget(m_settingsEdit);

    QPushButton* saveBtn = new QPushButton("Save Settings");
    layout->addWidget(saveBtn);

    m_tabWidget->addTab(settingsWidget, "Settings");
}

void MainWindow::setupProgressTab() {
    QWidget* progressWidget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(progressWidget);

    m_progressBar = new QProgressBar;
    layout->addWidget(m_progressBar);

    m_speedLabel = new QLabel("Speed: 0 MB/s");
    m_speedLabel->setObjectName("speedLabel");
    layout->addWidget(m_speedLabel);

    m_etaLabel = new QLabel("ETA: --");
    m_etaLabel->setObjectName("etaLabel");
    layout->addWidget(m_etaLabel);

    m_logEdit = new QTextEdit;
    m_logEdit->setObjectName("logEdit");
    m_logEdit->setReadOnly(true);
    layout->addWidget(m_logEdit);

    m_tabWidget->addTab(progressWidget, "Progress");
}

void MainWindow::closeEvent(QCloseEvent* event) {
    saveWindowState();
    event->accept();
}

void MainWindow::saveWindowState() {
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow::restoreWindowState() {
    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::updateLists() {
    m_activeList->clear();
    for (auto task : m_queue->activeTasks()) {
        m_activeList->addItem(task->source() + " -> " + task->destination());
    }

    m_waitingList->clear();
    for (auto task : m_queue->waitingTasks()) {
        m_waitingList->addItem(task->source() + " -> " + task->destination());
    }
}

void MainWindow::reorderUp() {
    int row = m_waitingList->currentRow();
    if (row > 0) {
        m_queue->reorderWaiting(row, row - 1);
        updateLists();
        m_waitingList->setCurrentRow(row - 1);
    }
}

void MainWindow::reorderDown() {
    int row = m_waitingList->currentRow();
    if (row >= 0 && row < m_waitingList->count() - 1) {
        m_queue->reorderWaiting(row, row + 1);
        updateLists();
        m_waitingList->setCurrentRow(row + 1);
    }
}

void MainWindow::addTask() {
    if (m_addTaskDialog->exec() == QDialog::Accepted) {
        TransferTask* task = m_addTaskDialog->getTask();
        m_queue->addTask(task);
        updateLists();
    }
}

void MainWindow::onDriveAdded(const QStorageInfo& drive) {
    int row = m_drivesTable->rowCount();
    m_drivesTable->insertRow(row);
    m_drivesTable->setItem(row, 0, new QTableWidgetItem(drive.name()));
    m_drivesTable->setItem(row, 1, new QTableWidgetItem(drive.rootPath()));
    m_drivesTable->setItem(row, 2, new QTableWidgetItem(QString::number(drive.bytesTotal() / (1024*1024*1024)) + " GB"));
    m_drivesTable->setItem(row, 3, new QTableWidgetItem(QString::number(drive.bytesAvailable() / (1024*1024*1024)) + " GB"));
}

void MainWindow::onDriveRemoved(const QStorageInfo& drive) {
    for (int i = 0; i < m_drivesTable->rowCount(); ++i) {
        if (m_drivesTable->item(i, 1)->text() == drive.rootPath()) {
            m_drivesTable->removeRow(i);
            break;
        }
    }
}

void MainWindow::onProgressUpdated(double speed, qint64 eta, const QString& log) {
    m_progressBar->setValue(50); // Placeholder
    m_speedLabel->setText(QString("Speed: %1 MB/s").arg(speed, 0, 'f', 2));
    m_etaLabel->setText(QString("ETA: %1 s").arg(eta));
    m_logEdit->append(log);
}

void MainWindow::onErrorAdded(const QString& error) {
    m_logEdit->append("ERROR: " + error);
    m_errorEdit->append("ERROR: " + error);
}