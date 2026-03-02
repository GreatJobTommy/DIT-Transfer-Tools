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

MainWindow::MainWindow(QueueManager* queue, QWidget* parent)
    : QMainWindow(parent), m_queue(queue) {
    m_driveMonitor = new DriveMonitor(this);
    m_progressMonitor = new ProgressMonitor(this);
    m_errorManager = new ErrorManager(this);
    m_settingsManager = new SettingsManager(this);
    m_notificationManager = new NotificationManager(this);

    setWindowTitle("DIT Transfer Tools v3.3");
    setWindowIcon(QIcon(":/icons/app.png")); // Assuming icons

    // Setup tray icon
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(windowIcon());
    m_trayIcon->setToolTip("DIT Transfer Tools v3.3");
    m_trayIcon->show();

    m_notificationManager->setTrayIcon(m_trayIcon);

    setupUI();
    setupHotkeys();

    // Connect signals
    connect(m_settingsManager, &SettingsManager::settingChanged, this, &MainWindow::settingChanged);
    connect(m_driveMonitor, &DriveMonitor::driveConnected, this, &MainWindow::updateDrives);
    connect(m_driveMonitor, &DriveMonitor::driveDisconnected, this, &MainWindow::updateDrives);

    // Connect notification settings
    m_notificationManager->setNotificationsEnabled(m_settingsManager->getNotificationsEnabled());
    m_notificationManager->setSoundEnabled(m_settingsManager->getNotificationSoundEnabled());
    m_notificationManager->setSoundFile(m_settingsManager->getNotificationSoundFile());
    m_notificationManager->setUseSystemNotify(m_settingsManager->getUseSystemNotifications());

    updateDrives();

    // Connect to queue for task notifications
    connect(m_queue, &QueueManager::taskCompleted, this, &MainWindow::onTaskCompleted);
    connect(m_queue, &QueueManager::taskFailed, this, &MainWindow::onTaskFailed);
    connect(m_queue, &QueueManager::taskPaused, this, &MainWindow::onTaskPaused);
    connect(m_driveMonitor, &DriveMonitor::driveReconnected, this, &MainWindow::onDriveReconnected);

    // For demo, add some tasks
    TransferTask* t1 = new TransferTask("src1", "dst1");
    TransferTask* t2 = new TransferTask("src2", "dst2");
    m_queue->addTask(t1);
    m_queue->addTask(t2);
    m_progressMonitor->addTask(t1);
    m_progressMonitor->addTask(t2);

    updateLists();
    updateDashboard();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    m_tabWidget = new QTabWidget;
    setCentralWidget(m_tabWidget);

    createDashboardTab();
    createQueueTab();
    createDrivesTab();
    createSettingsTab();
    createProgressTab();
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

    // Drives card
    QGroupBox* drivesCard = new QGroupBox("Drives");
    drivesCard->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout* drivesLayout = new QVBoxLayout(drivesCard);
    m_drivesCountLabel = new QLabel("Connected: 0");
    drivesLayout->addWidget(m_drivesCountLabel);

    // Progress card
    QGroupBox* progressCard = new QGroupBox("Progress");
    progressCard->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout* progressLayout = new QVBoxLayout(progressCard);
    m_progressLabel = new QLabel("Overall: 0%");
    progressLayout->addWidget(m_progressLabel);

    // Errors card
    QGroupBox* errorsCard = new QGroupBox("Errors");
    errorsCard->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout* errorsLayout = new QVBoxLayout(errorsCard);
    m_errorsCountLabel = new QLabel("Count: 0");
    errorsLayout->addWidget(m_errorsCountLabel);

    layout->addWidget(queueCard, 0, 0);
    layout->addWidget(drivesCard, 0, 1);
    layout->addWidget(progressCard, 1, 0);
    layout->addWidget(errorsCard, 1, 1);

    m_tabWidget->addTab(dashboard, QIcon(":/icons/dashboard.png"), "Dashboard");
}

void MainWindow::createQueueTab() {
    QWidget* queueTab = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(queueTab);

    QLabel* waitingLabel = new QLabel("Waiting Queue (Drag to reorder)");
    layout->addWidget(waitingLabel);

    m_waitingList = new DragDropList;
    m_waitingList->setObjectName("waitingList");
    m_waitingList->setToolTip("Drag items to reorder tasks");
    connect(m_waitingList, &DragDropList::orderChanged, [this]() {
        QList<TransferTask*> newOrder;
        for (int i = 0; i < m_waitingList->count(); ++i) {
            // Find task by matching text, simplistic
            QString text = m_waitingList->item(i)->text();
            for (auto task : m_queue->waitingTasks()) {
                if (task->source() + " -> " + task->destination() == text) {
                    newOrder.append(task);
                    break;
                }
            }
        }
        m_queue->setWaitingOrder(newOrder);
    });
    layout->addWidget(m_waitingList);

    QLabel* dropLabel = new QLabel("Drop files/folders here to add tasks");
    dropLabel->setAlignment(Qt::AlignCenter);
    dropLabel->setStyleSheet("border: 2px dashed #aaa; padding: 20px; font-size: 14pt;");
    layout->addWidget(dropLabel);

    m_dropZone = new DragDropList;
    m_dropZone->setMinimumHeight(100);
    m_dropZone->setToolTip("Drop files or folders here to create transfer tasks");
    connect(m_dropZone, &DragDropList::filesDropped, this, &MainWindow::onFilesDropped);
    layout->addWidget(m_dropZone);

    m_tabWidget->addTab(queueTab, QIcon(":/icons/queue.png"), "Queue");
}

void MainWindow::createDrivesTab() {
    QWidget* drivesTab = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(drivesTab);

    QLabel* drivesLabel = new QLabel("Connected Drives");
    layout->addWidget(drivesLabel);

    m_drivesList = new QListWidget;
    m_drivesList->setObjectName("drivesList");
    layout->addWidget(m_drivesList);

    QPushButton* ejectBtn = new QPushButton("Eject Selected");
    ejectBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_TrashIcon));
    connect(ejectBtn, &QPushButton::clicked, this, &MainWindow::ejectDrive);
    layout->addWidget(ejectBtn);

    m_tabWidget->addTab(drivesTab, QIcon(":/icons/drive.png"), "Drives");
}

void MainWindow::createSettingsTab() {
    QWidget* settingsTab = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(settingsTab);

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

    layout->addStretch();

    m_tabWidget->addTab(settingsTab, QIcon(":/icons/settings.png"), "Settings");
}

void MainWindow::createProgressTab() {
    QWidget* progressTab = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(progressTab);

    QLabel* progressLabel = new QLabel("Overall Progress");
    layout->addWidget(progressLabel);

    m_overallProgress = new QProgressBar;
    m_overallProgress->setObjectName("overallProgress");
    layout->addWidget(m_overallProgress);

    QLabel* logLabel = new QLabel("Transfer Logs");
    layout->addWidget(logLabel);

    m_logView = new QTextEdit;
    m_logView->setReadOnly(true);
    layout->addWidget(m_logView);

    m_tabWidget->addTab(progressTab, QIcon(":/icons/progress.png"), "Progress");
}

void MainWindow::updateLists() {
    m_waitingList->clear();
    for (auto task : m_queue->waitingTasks()) {
        QListWidgetItem* item = new QListWidgetItem(task->source() + " -> " + task->destination());
        item->setToolTip("Drag to reorder");
        m_waitingList->addItem(item);
    }
}

void MainWindow::addTask() {
    AddTaskDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        TransferTask* task = dialog.getTask();
        m_queue->addTask(task);
        m_progressMonitor->addTask(task);
        updateLists();
        updateDashboard();
    }
}

void MainWindow::onFilesDropped(const QStringList& files) {
    for (const QString& file : files) {
        // Create task from dropped file
        TransferTask* task = new TransferTask(file, "/tmp/dest"); // Placeholder dest
        m_queue->addTask(task);
        m_progressMonitor->addTask(task);
    }
    updateLists();
    updateDashboard();
}

void MainWindow::ejectDrive() {
    // Placeholder for eject
    qDebug() << "Eject drive";
}

void MainWindow::updateDashboard() {
    int active = m_queue->activeTasks().size();
    int waiting = m_queue->waitingTasks().size();
    m_queueCountLabel->setText(QString("Active: %1\nWaiting: %2").arg(active).arg(waiting));

    int drives = m_driveMonitor->getCurrentDrives().size();
    m_drivesCountLabel->setText(QString("Connected: %1").arg(drives));

    // Calculate overall progress
    auto progress = m_progressMonitor->getProgress();
    int total = 0;
    for (int p : progress.values()) {
        total += p;
    }
    int avg = progress.isEmpty() ? 0 : total / progress.size();
    m_progressLabel->setText(QString("Overall: %1%").arg(avg));
    m_overallProgress->setValue(avg);

    int errors = m_errorManager->getErrors().size();
    m_errorsCountLabel->setText(QString("Count: %1").arg(errors));
}

void MainWindow::updateProgress() {
    updateDashboard();
}

void MainWindow::updateDrives() {
    m_drivesList->clear();
    for (const QStorageInfo& info : m_driveMonitor->getCurrentDrives()) {
        if (info.isReady()) {
            QListWidgetItem* item = new QListWidgetItem(info.rootPath() + " (" + QString::number(info.bytesAvailable() / 1024 / 1024) + " MB free)");
            item->setIcon(QApplication::style()->standardIcon(QStyle::SP_DriveHDIcon));
            m_drivesList->addItem(item);
        }
    }
}

void MainWindow::updateErrors() {
    updateDashboard();
}

void MainWindow::settingChanged(const QString& key, const QVariant& value) {
    if (key == "maxParallel") {
        // Update parallel manager or something
    } else if (key.startsWith("notifications/")) {
        // Update notification settings
        m_notificationManager->setNotificationsEnabled(m_settingsManager->getNotificationsEnabled());
        m_notificationManager->setSoundEnabled(m_settingsManager->getNotificationSoundEnabled());
        m_notificationManager->setSoundFile(m_settingsManager->getNotificationSoundFile());
        m_notificationManager->setUseSystemNotify(m_settingsManager->getUseSystemNotifications());
    }
}

void MainWindow::onTaskCompleted(TransferTask* task) {
    if (!task) return;
    QString title = "Transfer Completed";
    QString message = QString("Task '%1' to '%2' completed successfully.")
                     .arg(task->source(), task->destination());
    m_notificationManager->showNotification(title, message, QSystemTrayIcon::Information);
}

void MainWindow::onTaskFailed(TransferTask* task) {
    if (!task) return;
    QString title = "Transfer Failed";
    QString message = QString("Task '%1' to '%2' failed.")
                     .arg(task->source(), task->destination());
    m_notificationManager->showNotification(title, message, QSystemTrayIcon::Critical);
}

void MainWindow::onTaskPaused(TransferTask* task) {
    if (!task) return;
    QString title = "Transfer Paused";
    QString message = QString("Task '%1' to '%2' has been paused.")
                     .arg(task->source(), task->destination());
    m_notificationManager->showNotification(title, message, QSystemTrayIcon::Warning);
}

void MainWindow::onDriveReconnected() {
    QString title = "Drive Reconnected";
    QString message = "A previously disconnected drive has been reconnected.";
    m_notificationManager->showNotification(title, message, QSystemTrayIcon::Information);
}