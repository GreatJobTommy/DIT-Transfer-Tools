#include "MainWindow.h"
#include "TransferTask.h"
#include <QLabel>
#include <QApplication>
#include <QStyle>
#include <QIcon>
#include <QShortcut>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QTextEdit>
#include <QSlider>
#include <QProgressBar>
#include <QStorageInfo>
#include <QDebug>
#include <QCheckBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QProcess>
#include <QStandardPaths>
#include <QComboBox>
#include <QMenu>
#include <QListWidget>
//#include <QtCharts/QChartView>

MainWindow::MainWindow(QueueManager* queue, QWidget* parent)
    : QMainWindow(parent), m_queue(queue) {
    m_driveMonitor = new DriveMonitor(this);
    m_progressMonitor = new ProgressMonitor(this);
    m_settingsManager = new SettingsManager(this);

    setWindowTitle("DIT Transfer Tools v4.0");
    setWindowIcon(QIcon(":/icons/app.png")); // Assuming icons

    // Setup tray icon
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(windowIcon());
    m_trayIcon->setToolTip("DIT Transfer Tools v4.0");
    m_trayIcon->show();

    setupUI();
    setupHotkeys();

    // Connect signals
    connect(m_settingsManager, &SettingsManager::settingChanged, this, &MainWindow::settingChanged);

    // Connect to queue for task notifications
    connect(m_queue, &QueueManager::taskCompleted, this, &MainWindow::onTaskCompleted);
    connect(m_queue, &QueueManager::taskFailed, this, &MainWindow::onTaskFailed);
    connect(m_queue, &QueueManager::taskPaused, this, &MainWindow::onTaskPaused);
    // connect(m_driveMonitor, QOverload<const QString&, const QString&, qint64>::of(&DriveMonitor::driveReconnected), this, &MainWindow::onDriveReconnected);

    updateLists();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    m_tabWidget = new QTabWidget;
    setCentralWidget(m_tabWidget);

    createDashboardTab();
    createQueueTab();
    createDrivesTab();
    createSettingsTab();

    // Remove Progress tab as per task
    // createProgressTab(); // Not implemented, removing

    // Node tab
    createNodeTab();
}

void MainWindow::setupHotkeys() {
    m_addTaskAction = new QAction("Add Task", this);
    m_addTaskAction->setShortcut(QKeySequence("Ctrl+A"));
    connect(m_addTaskAction, &QAction::triggered, this, &MainWindow::addTask);
    addAction(m_addTaskAction);
}

void MainWindow::createDashboardTab() {
    QWidget* dashboard = new QWidget;
    QGridLayout* layout = new QGridLayout(dashboard);

    // Queue card
    QGroupBox* queueCard = new QGroupBox("Queue");
    queueCard->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout* queueLayout = new QVBoxLayout(queueCard);
    m_queueCountLabel = new QLabel("Active: 0\nWaiting: 0");
    m_queueCountLabel->setObjectName("queueCountLabel");
    queueLayout->addWidget(m_queueCountLabel);
    layout->addWidget(queueCard, 0, 0);

    // Drives card
    QGroupBox* drivesCard = new QGroupBox("Drives");
    drivesCard->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout* drivesLayout = new QVBoxLayout(drivesCard);
    m_drivesCountLabel = new QLabel("Connected: 0");
    drivesLayout->addWidget(m_drivesCountLabel);

    // USB ports info
    QLabel* usbLabel = new QLabel("USB Ports:");
    QListWidget* usbList = new QListWidget;
    usbList->setMaximumHeight(100);
    // Populate with max speed, recommend, colors
    // Placeholder
    usbList->addItem("Port 1: USB 3.0 - Max 5Gbps - Recommended for fast drives");
    usbList->addItem("Port 2: USB 2.0 - Max 480Mbps - Use for slow devices");
    drivesLayout->addWidget(usbLabel);
    drivesLayout->addWidget(usbList);
    layout->addWidget(drivesCard, 0, 1);

    // Progress card
    QGroupBox* progressCard = new QGroupBox("Progress");
    progressCard->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout* progressLayout = new QVBoxLayout(progressCard);
    m_progressLabel = new QLabel("Overall: 0%");
    progressLayout->addWidget(m_progressLabel);

    m_overallProgress = new QProgressBar;
    m_overallProgress->setObjectName("overallProgress");
    progressLayout->addWidget(m_overallProgress);
    layout->addWidget(progressCard, 1, 0);

    // Errors card
    QGroupBox* errorsCard = new QGroupBox("Errors");
    errorsCard->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout* errorsLayout = new QVBoxLayout(errorsCard);
    m_errorsCountLabel = new QLabel("Count: 0");
    errorsLayout->addWidget(m_errorsCountLabel);
    layout->addWidget(errorsCard, 1, 1);

    m_tabWidget->addTab(dashboard, QIcon(":/icons/dashboard.png"), "Dashboard");
}

void MainWindow::createQueueTab() {
    QWidget* queueTab = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(queueTab);

    // Hash dropdown
    QHBoxLayout* hashLayout = new QHBoxLayout;
    QLabel* hashLabel = new QLabel("Hash:");
    QComboBox* hashCombo = new QComboBox;
    hashCombo->addItems({"MD5", "SHA256", "SHA512"});
    hashLayout->addWidget(hashLabel);
    hashLayout->addWidget(hashCombo);
    hashLayout->addStretch();
    layout->addLayout(hashLayout);

    // Queue list
    m_waitingList = new QListWidget;
    m_waitingList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_waitingList, &QListWidget::customContextMenuRequested, this, &MainWindow::showQueueContextMenu);
    layout->addWidget(new QLabel("Active Queue Tasks:"));
    layout->addWidget(m_waitingList);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout;
    QPushButton* addBtn = new QPushButton("Add");
    QPushButton* multiAddBtn = new QPushButton("+Multi");
    QPushButton* startBtn = new QPushButton("Start");
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(multiAddBtn);
    btnLayout->addWidget(startBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    connect(addBtn, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(multiAddBtn, &QPushButton::clicked, this, &MainWindow::addMultiTask);
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::startQueue);

    m_tabWidget->addTab(queueTab, QIcon(":/icons/queue.png"), "Queue");
}

void MainWindow::createDrivesTab() {
    QWidget* drivesTab = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(drivesTab);

    m_drivesList = new QListWidget;
    m_drivesList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_drivesList, &QListWidget::customContextMenuRequested, this, &MainWindow::showDrivesContextMenu);
    layout->addWidget(new QLabel("Drives:"));
    layout->addWidget(m_drivesList);

    // Speedtest button
    QPushButton* speedtestBtn = new QPushButton("Speedtest");
    speedtestBtn->setStyleSheet("QPushButton { background-color: green; color: white; }"); // Color as per task
    connect(speedtestBtn, &QPushButton::clicked, this, &MainWindow::runSpeedtest);
    layout->addWidget(speedtestBtn);

    m_tabWidget->addTab(drivesTab, QIcon(":/icons/drives.png"), "Drives");
}

void MainWindow::createNodeTab() {
    QWidget* nodeTab = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(nodeTab);

    // Placeholder for graph flows to queue
    QLabel* graphLabel = new QLabel("Node Graph Flows to Queue");
    layout->addWidget(graphLabel);

    // TODO: Implement graph

    m_tabWidget->addTab(nodeTab, QIcon(":/icons/node.png"), "Node");
}

void MainWindow::createSettingsTab() {
    QWidget* settingsTab = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(settingsTab);

    // Theme
    QHBoxLayout* themeLayout = new QHBoxLayout;
    QLabel* themeLabel = new QLabel("Theme:");
    QComboBox* themeCombo = new QComboBox;
    themeCombo->addItems({"Light", "Dark"});
    themeLayout->addWidget(themeLabel);
    themeLayout->addWidget(themeCombo);
    themeLayout->addStretch();
    layout->addLayout(themeLayout);

    connect(themeCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged), this, &MainWindow::changeTheme);

    // Parallel transfers
    QLabel* parallelLabel = new QLabel("Max Parallel Transfers");
    layout->addWidget(parallelLabel);

    m_parallelSlider = new QSlider(Qt::Horizontal);
    m_parallelSlider->setObjectName("parallelSlider");
    m_parallelSlider->setRange(1, 10);
    m_parallelSlider->setValue(m_settingsManager->getMaxParallelTransfers());
    connect(m_parallelSlider, &QSlider::valueChanged, [this](int value) {
        m_settingsManager->setMaxParallelTransfers(value);
        m_parallelLabel->setText(QString("Max Parallel Transfers: %1").arg(value));
    });
    layout->addWidget(m_parallelSlider);

    m_parallelLabel = new QLabel(QString("Max Parallel Transfers: %1").arg(m_parallelSlider->value()));
    layout->addWidget(m_parallelLabel);

    layout->addSpacing(20);

    // Notification settings
    QGroupBox* notificationGroup = new QGroupBox("Notifications");
    notificationGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout* notificationLayout = new QVBoxLayout(notificationGroup);

    // Enable notifications
    QCheckBox* notificationsCheck = new QCheckBox("Enable notifications");
    notificationsCheck->setChecked(m_settingsManager->getNotificationsEnabled());
    connect(notificationsCheck, &QCheckBox::toggled, [this](bool checked) {
        m_settingsManager->setNotificationsEnabled(checked);
    });
    notificationLayout->addWidget(notificationsCheck);

    // Enable sound
    QCheckBox* soundCheck = new QCheckBox("Enable notification sounds");
    soundCheck->setChecked(m_settingsManager->getNotificationSoundEnabled());
    connect(soundCheck, &QCheckBox::toggled, [this](bool checked) {
        m_settingsManager->setNotificationSoundEnabled(checked);
    });
    notificationLayout->addWidget(soundCheck);

    // Sound file
    QHBoxLayout* soundFileLayout = new QHBoxLayout;
    QLabel* soundFileLabel = new QLabel("Sound file:");
    QLineEdit* soundFileEdit = new QLineEdit(m_settingsManager->getNotificationSoundFile());
    QPushButton* soundFileButton = new QPushButton("Browse...");
    soundFileLayout->addWidget(soundFileLabel);
    soundFileLayout->addWidget(soundFileEdit);
    soundFileLayout->addWidget(soundFileButton);

    connect(soundFileEdit, &QLineEdit::textChanged, [this](const QString& text) {
        m_settingsManager->setNotificationSoundFile(text);
    });

    connect(soundFileButton, &QPushButton::clicked, [soundFileEdit, this]() {
        QString file = QFileDialog::getOpenFileName(nullptr, "Select Sound File",
                                                   QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
                                                   "Audio Files (*.wav *.ogg *.mp3 *.aiff);;All Files (*)");
        if (!file.isEmpty()) {
            soundFileEdit->setText(file);
        }
    });

    notificationLayout->addLayout(soundFileLayout);

    // Use system notifications
    QCheckBox* systemNotifyCheck = new QCheckBox("Use system notifications (notify-send/DBUS)");
    systemNotifyCheck->setChecked(m_settingsManager->getUseSystemNotifications());
    connect(systemNotifyCheck, &QCheckBox::toggled, [this](bool checked) {
        m_settingsManager->setUseSystemNotifications(checked);
    });
    notificationLayout->addWidget(systemNotifyCheck);

    layout->addWidget(notificationGroup);

    layout->addSpacing(20);

    // Logging settings
    QGroupBox* loggingGroup = new QGroupBox("Logging");
    loggingGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout* loggingLayout = new QVBoxLayout(loggingGroup);

    // Log level
    QHBoxLayout* levelLayout = new QHBoxLayout;
    QLabel* levelLabel = new QLabel("Log Level:");
    QComboBox* levelCombo = new QComboBox;
    levelCombo->addItem("DEBUG", 0);
    levelCombo->addItem("INFO", 1);
    levelCombo->addItem("WARNING", 2);
    levelCombo->addItem("ERROR", 3);
    levelCombo->addItem("CRITICAL", 4);
    levelCombo->setCurrentIndex(m_settingsManager->getLogLevel());
    connect(levelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, levelCombo](int index) {
        m_settingsManager->setLogLevel(levelCombo->itemData(index).toInt());
    });
    levelLayout->addWidget(levelLabel);
    levelLayout->addWidget(levelCombo);
    levelLayout->addStretch();
    loggingLayout->addLayout(levelLayout);

    // Log path
    QHBoxLayout* pathLayout = new QHBoxLayout;
    QLabel* pathLabel = new QLabel("Log File:");
    QLineEdit* pathEdit = new QLineEdit(m_settingsManager->getLogPath());
    QPushButton* pathButton = new QPushButton("Browse...");
    pathLayout->addWidget(pathLabel);
    pathLayout->addWidget(pathEdit);
    pathLayout->addWidget(pathButton);

    connect(pathEdit, &QLineEdit::textChanged, [this](const QString& text) {
        m_settingsManager->setLogPath(text);
    });

    connect(pathButton, &QPushButton::clicked, [pathEdit, this]() {
        QString file = QFileDialog::getSaveFileName(nullptr, "Select Log File",
                                                   pathEdit->text(),
                                                   "Log Files (*.log);;All Files (*)");
        if (!file.isEmpty()) {
            pathEdit->setText(file);
        }
    });

    loggingLayout->addLayout(pathLayout);

    layout->addWidget(loggingGroup);

    layout->addStretch();

    m_tabWidget->addTab(settingsTab, QIcon(":/icons/settings.png"), "Settings");
}



void MainWindow::updateLists() {
    m_waitingList->clear();

    // Active tasks
    for (auto task : m_queue->activeTasks()) {
        QString progress = QString("%1%").arg(task->progress());
        QString statusStr;
        switch (task->status()) {
        case TransferStatus::Pending: statusStr = "Pending"; break;
        case TransferStatus::Active: statusStr = "Active"; break;
        case TransferStatus::Paused: statusStr = "Paused"; break;
        case TransferStatus::Completed: statusStr = "Completed"; break;
        case TransferStatus::Failed: statusStr = "Failed"; break;
        }
        QString remaining = QString("%1 GB").arg(task->remainingSize() / 1e9, 0, 'f', 2);
        QString eta = QString("%1s").arg(task->eta());
        QString text = QString("%1 -> %2 | %3 | %4 | %5 | ETA: %6")
                       .arg(task->source(), task->destination(), progress, statusStr, remaining, eta);
        QListWidgetItem* item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, QVariant::fromValue(task)); // Store task pointer
        m_waitingList->addItem(item);
    }

    // Waiting tasks
    for (auto task : m_queue->waitingTasks()) {
        QListWidgetItem* item = new QListWidgetItem(task->source() + " -> " + task->destination() + " | Waiting");
        item->setData(Qt::UserRole, QVariant::fromValue(task));
        m_waitingList->addItem(item);
    }
}

void MainWindow::addTask() {
    // For minimal test, add a dummy task
    static int count = 1;
    TransferTask* task = new TransferTask(QString("test_src_%1").arg(count), QString("test_dst_%1").arg(count));
    count++;
    m_queue->addTask(task);
    m_progressMonitor->addTask(task);
    updateLists();
}

void MainWindow::onFilesDropped(const QStringList& files) {
    for (const QString& file : files) {
        // Create task from dropped file
        TransferTask* task = new TransferTask(file, "/tmp/dest"); // Placeholder dest
        m_queue->addTask(task);
        m_progressMonitor->addTask(task);
    }
    updateLists();
}

void MainWindow::ejectDrive() {
    QListWidgetItem* item = m_drivesList->currentItem();
    if (!item) return;
    QString text = item->text();
    int idx = text.indexOf(": ");
    if (idx == -1) return;
    QString path = text.mid(idx + 2);
    idx = path.indexOf(" (");
    if (idx != -1) path = path.left(idx);
    // Eject using umount
    QProcess::execute("umount", QStringList() << path);
}



void MainWindow::updateProgress() {
}

void MainWindow::onProgressChanged(qint64 bytes, qint64 speed, qint64 eta) {
    // Minimal: just debug
    qDebug() << "Progress:" << bytes << "bytes," << speed << "B/s";
}





void MainWindow::settingChanged(const QString& key, const QVariant& value) {
    if (key == "maxParallel") {
        // Update parallel manager or something
    }
}

void MainWindow::onTaskCompleted(TransferTask* task) {
    if (!task) return;
    qDebug() << "Task completed:" << task->source() << "to" << task->destination();

    // Create log file per dest
    QString logDir = task->destination() + "/logs";
    QDir().mkpath(logDir);
    QString logFile = QString("%1/%2_%3.txt").arg(logDir, QDateTime::currentDateTime().toString("yyyy-MM-dd"), task->hash());
    QFile file(logFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Date: " << QDateTime::currentDateTime().toString() << "\n";
        out << "Hash: " << task->hash() << "\n";
        out << "Compare: " << (task->hashVerified() ? "OK" : "Failed") << "\n";
        out << "Status: Completed\n";
        out << "Duration: " << task->duration() << "s\n";
        out << "Source: " << task->source() << "\n";
        out << "Destination: " << task->destination() << "\n";
    }
}

void MainWindow::onTaskFailed(TransferTask* task) {
    if (!task) return;
    qDebug() << "Task failed:" << task->source() << "to" << task->destination();
}

void MainWindow::onTaskPaused(TransferTask* task) {
    if (!task) return;
    qDebug() << "Task paused:" << task->source() << "to" << task->destination();
}

void MainWindow::onDriveReconnected() {
    qDebug() << "Drive reconnected";
}

void MainWindow::addMultiTask() {
    // TODO: Implement multi-add
    qDebug() << "Add multi task";
}

void MainWindow::startQueue() {
    // TODO: Start all waiting tasks
    qDebug() << "Start queue";
}

void MainWindow::runSpeedtest() {
    // TODO: Run speedtest on selected drive
    qDebug() << "Run speedtest";
}

void MainWindow::showQueueContextMenu(const QPoint& pos) {
    QListWidgetItem* item = m_waitingList->itemAt(pos);
    if (!item) return;

    QMenu* menu = new QMenu(this);
    menu->addAction("Pause", this, [this]() { /* pause task */ });
    menu->addAction("Resume", this, [this]() { /* resume task */ });
    menu->addAction("Cancel", this, [this]() { /* cancel task */ });
    menu->addAction("Remove", this, [this]() { /* remove task */ });
    menu->addAction("Log", this, [this]() { /* show log */ });
    menu->exec(m_waitingList->mapToGlobal(pos));
}

void MainWindow::showDrivesContextMenu(const QPoint& pos) {
    QListWidgetItem* item = m_drivesList->itemAt(pos);
    if (!item) return;

    QMenu* menu = new QMenu(this);
    menu->addAction("Speedtest Read", this, [this]() { /* speedtest read */ });
    menu->addAction("Speedtest Write", this, [this]() { /* speedtest write */ });
    menu->addAction("Cancel", this, [this]() { /* cancel */ });
    menu->exec(m_drivesList->mapToGlobal(pos));
}

void MainWindow::changeTheme(const QString& theme) {
    if (theme == "Dark") {
        qApp->setStyleSheet("QMainWindow { background-color: #2b2b2b; color: #ffffff; }");
    } else {
        qApp->setStyleSheet("");
    }
}