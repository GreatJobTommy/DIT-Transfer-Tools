<<<<<<< HEAD
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ParallelManager.h"
#include "AddTaskDialog.h"
#include "QueueManager.h"
#include <QFileDialog>
#include <QSettings>
#include <QDebug>
#include <QSlider>
#include <QVBoxLayout>
#include <QLabel>
#include <QDropEvent>
#include <QMimeData>
=======
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QSettings>
#include <QDebug>
#include <QStandardPaths>
#include <QFileInfo>
>>>>>>> origin/main

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUI();

    // Live Preview Setup
    previewScene = new PreviewScene(this);
    previewView = new PreviewView(previewScene, this);
    previewWidget = new QWidget(this);
    previewWidget->setLayout(new QVBoxLayout());
    previewWidget->layout()->addWidget(previewView);
    ui->verticalLayout->insertWidget(1, previewWidget);
    
    parallelMgr = new ParallelManager(this);
    queueMgr = new QueueManager(this);
    driveMon = new DriveMonitor(this);
    hashMgr = new HashManager(this);
    errorMgr = new ErrorManager(this);
    
    setupConnections();
    loadSettings();
    
    statusTimer = new QTimer(this);
    connect(statusTimer, &amp;QTimer::timeout, this, &amp;MainWindow::updateQueueStatus);
    statusTimer->start(1000);
    
<<<<<<< HEAD
    // Thread slider setup
    threadSlider = new QSlider(Qt::Horizontal, this);
    threadSlider->setRange(1, 32);
    threadSlider->setValue(parallelMgr->maxThreads());
    connect(threadSlider, &amp;QSlider::valueChanged, this, &amp;MainWindow::onThreadCountChanged);
    ui->statusBar()->addPermanentWidget(threadSlider);
    ui->statusBar()->addWidget(new QLabel("Threads: "));
    ui->statusBar()->addWidget(new QLabel("1")); // Placeholder, update dynamically
    
    connect(driveMon, &amp;DriveMonitor::driveDisconnected, parallelMgr, [this]() { parallelMgr->pause(); });
    connect(driveMon, &amp;DriveMonitor::driveConnected, parallelMgr, &amp;ParallelManager::resume);
=======
    connect(driveMon, &amp;DriveMonitor::driveDisconnected, queueMgr, &amp;QueueManager::pauseAll);
    connect(driveMon, &amp;DriveMonitor::driveConnected, queueMgr, &amp;QueueManager::resumeAll);
    connect(driveMon, &amp;DriveMonitor::drivesChanged, this, [this](const QStringList &amp;drives){
        ui->statusBar()->showMessage(QString("Drives: %1").arg(drives.join(", ")), 2000);
    });
    connect(errorMgr, &amp;ErrorManager::errorOccurred, this, &amp;MainWindow::onErrorOccurred);
}

void MainWindow::onErrorOccurred(ErrorCategory cat, const QString&amp; message) {
    errorMgr->showUserDialog(cat, message);
>>>>>>> origin/main
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

<<<<<<< HEAD
void MainWindow::setupUI() {
    // Placeholder
}

void MainWindow::setupConnections() {
    connect(ui->addTaskButton, &amp;QPushButton::clicked, this, &amp;MainWindow::on_addTaskButton_clicked);
    
    // Preview integration with QueueManager and ParallelManager
    connect(queueMgr, &amp;QueueManager::taskStatusChanged, this, [this](TransferTask*, TaskStatus) {
        previewScene->setQueueInfo(queueMgr->getTasks().size(), parallelMgr->activeThreads());
    });
    connect(parallelMgr, &amp;ParallelManager::progressUpdated, previewScene, &amp;PreviewScene::updateSpeedGraph);
    // Assume ParallelManager emits progressUpdated(double speed)
    // Similar for chunk flow, ETA
    
    // Drag drop for MainWindow and children
    setAcceptDrops(true);
    
    // Connect ParallelManager signals
    connect(parallelMgr, &amp;ParallelManager::taskStarted, this, [](TransferTask* t) {
        qDebug() << "Task started:" << t;
    });
    connect(parallelMgr, &amp;ParallelManager::taskFinished, this, &amp;MainWindow::onTaskFinished);
    
    // Connect QueueManager to list updates
    connect(queueMgr, &amp;QueueManager::taskStatusChanged, this, &amp;MainWindow::updateQueueStatus);
}

void MainWindow::on_addTaskButton_clicked() {
    AddTaskDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList sources = dialog.getSources();
        QStringList targets = dialog.getTargets();
        QJsonObject presets = dialog.getPresetSettings();
        
        // Create TransferTask(s)
        for (const QString&amp; src : sources) {
            TransferTask *task = new TransferTask(src, targets);
            // Apply presets
            // task->setPreset(presets); assume method
            queueMgr->addTask(task);
        }
        updateQueueStatus();
    }
}

void MainWindow::onThreadCountChanged(int value) {
    parallelMgr->setMaxThreads(value);
    queueMgr->setMaxParallel(value);
    qDebug() << "Threads set to" << value;
}

void MainWindow::updateQueueStatus() {
    ui->queueList->clear();
    QList<TransferTask*> tasks = queueMgr->getTasks();
    for (TransferTask *task : tasks) {
        QString itemText = QString("%1 -> %2").arg(task->source().split('/').last(), task->targets().join(","));
        ui->queueList->addItem(itemText);
    }
    int active = parallelMgr->activeThreads();
    ui->statusBar()->showMessage(QString("Active threads: %1 | Queue: %2").arg(active).arg(tasks.size()));
}

// Placeholder slots
void MainWindow::onTaskProgress(double, double, QString) {}
void MainWindow::onTaskFinished(bool) {}
void MainWindow::onVerification(bool) {}
void MainWindow::onProgressUpdated(double, double, QString) {}
void MainWindow::loadSettings() {}
void MainWindow::saveSettings() {}

// Drag drop for MainWindow
void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    QList<QUrl> urls = event->mimeData()->urls();
    QStringList sources;
    for (const QUrl&amp; url : urls) {
        QString path = url.toLocalFile();
        if (!path.isEmpty()) sources << path;
    }
    if (!sources.isEmpty()) {
        AddTaskDialog dialog(this);
        dialog.setSources(sources); // Assume setSources method added
        if (dialog.exec() == QDialog::Accepted) {
            // Add as above
            for (const QString&amp; src : sources) {
                TransferTask *task = new TransferTask(src, dialog.getTargets());
                queueMgr->addTask(task);
            }
            updateQueueStatus();
        }
    }
    event->acceptProposedAction();
=======
void MainWindow::onParallelChanged(int value) {
    Settings::parallel() = value;
    queueMgr->setMaxParallel(value);
>>>>>>> origin/main
}