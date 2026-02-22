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
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QSettings>
#include <QDebug>
#include <QStandardPaths>
#include <QFileInfo>
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
    connect(statusTimer, &QTimer::timeout, this, &MainWindow::updateQueueStatus);
    statusTimer->start(1000);
    
    // Thread slider setup
    threadSlider = new QSlider(Qt::Horizontal, this);
    threadSlider->setRange(1, 32);
    threadSlider->setValue(parallelMgr->maxThreads());
    connect(threadSlider, &QSlider::valueChanged, this, &MainWindow::onThreadCountChanged);
    ui->statusBar()->addPermanentWidget(threadSlider);
    ui->statusBar()->addWidget(new QLabel("Threads: "));
    ui->statusBar()->addWidget(new QLabel("1")); // Placeholder, update dynamically
    
    connect(driveMon, &DriveMonitor::driveDisconnected, parallelMgr, [this]() { parallelMgr->pause(); });
    connect(driveMon, &DriveMonitor::driveConnected, parallelMgr, &ParallelManager::resume);
    connect(driveMon, &DriveMonitor::driveDisconnected, queueMgr, &QueueManager::pauseAll);
    connect(driveMon, &DriveMonitor::driveConnected, queueMgr, &QueueManager::resumeAll);
    connect(driveMon, &DriveMonitor::drivesChanged, this, [this](const QStringList &drives){
        ui->statusBar()->showMessage(QString("Drives: %1").arg(drives.join(", ")), 2000);
    });
    connect(errorMgr, &ErrorManager::errorOccurred, this, &MainWindow::onErrorOccurred);
}
void MainWindow::onErrorOccurred(ErrorCategory cat, const QString& message) {
    errorMgr->showUserDialog(cat, message);
}
MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}
void MainWindow::setupUI() {
    // Placeholder
}
void MainWindow::setupConnections() {
    connect(ui->addTaskButton, &QPushButton::clicked, this, &MainWindow::on_addTaskButton_clicked);
    
    // Preview integration with QueueManager and ParallelManager
    connect(queueMgr, &QueueManager::taskStatusChanged, this, [this](TransferTask*, TaskStatus) {
        previewScene->setQueueInfo(queueMgr->getTasks().size(), parallelMgr->activeThreads());
    });
    connect(parallelMgr, &ParallelManager::progressUpdated, previewScene, &PreviewScene::updateSpeedGraph);
    // Assume ParallelManager emits progressUpdated(double speed)
    // Similar for chunk flow, ETA
    
    // Drag drop for MainWindow and children
    setAcceptDrops(true);
    
    // Connect ParallelManager signals
    connect(parallelMgr, &ParallelManager::taskStarted, this, [](TransferTask* t) {
        qDebug() << "Task started:" << t;
    });
    connect(parallelMgr, &ParallelManager::taskFinished, this, &MainWindow::onTaskFinished);
    
    // Connect QueueManager to list updates
    connect(queueMgr, &QueueManager::taskStatusChanged, this, &MainWindow::updateQueueStatus);
}
void MainWindow::on_addTaskButton_clicked() {
    AddTaskDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList sources = dialog.getSources();
        QStringList targets = dialog.getTargets();
        QJsonObject presets = dialog.getPresetSettings();
        
        // Create TransferTask(s)
        for (const QString& src : sources) {
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
    for (const QUrl& url : urls) {
        QString path = url.toLocalFile();
        if (!path.isEmpty()) sources << path;
    }
    if (!sources.isEmpty()) {
        AddTaskDialog dialog(this);
        dialog.setSources(sources); // Assume setSources method added
        if (dialog.exec() == QDialog::Accepted) {
            // Add as above
            for (const QString& src : sources) {
                TransferTask *task = new TransferTask(src, dialog.getTargets());
                queueMgr->addTask(task);
            }
            updateQueueStatus();
        }
    }
    event->acceptProposedAction();
void MainWindow::onParallelChanged(int value) {
    Settings::parallel() = value;
    queueMgr->setMaxParallel(value);
}