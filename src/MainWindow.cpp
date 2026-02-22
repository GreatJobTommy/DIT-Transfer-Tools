<<<<<<< HEAD
#include &quot;MainWindow.h&quot;
#include &quot;ui_MainWindow.h&quot;
#include &quot;ParallelManager.h&quot;
#include &quot;AddTaskDialog.h&quot;
#include &quot;QueueManager.h&quot;
#include &lt;QFileDialog&gt;
#include &lt;QSettings&gt;
#include &lt;QDebug&gt;
#include &lt;QSlider&gt;
#include &lt;QVBoxLayout&gt;
#include &lt;QLabel&gt;
#include &lt;QDropEvent&gt;
#include &lt;QMimeData&gt;
=======
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include &lt;QFileDialog&gt;
#include &lt;QSettings&gt;
#include &lt;QDebug&gt;
#include &lt;QStandardPaths&gt;
#include &lt;QFileInfo&gt;
>>>>>>> origin/main

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui-&gt;setupUi(this);
    setupUI();

    // Live Preview Setup
    previewScene = new PreviewScene(this);
    previewView = new PreviewView(previewScene, this);
    previewWidget = new QWidget(this);
    previewWidget-&gt;setLayout(new QVBoxLayout());
    previewWidget-&gt;layout()-&gt;addWidget(previewView);
    ui-&gt;verticalLayout-&gt;insertWidget(1, previewWidget);
    
    parallelMgr = new ParallelManager(this);
    queueMgr = new QueueManager(this);
    driveMon = new DriveMonitor(this);
    hashMgr = new HashManager(this);
    errorMgr = new ErrorManager(this);
    
    setupConnections();
    loadSettings();
    
    statusTimer = new QTimer(this);
    connect(statusTimer, &amp;QTimer::timeout, this, &amp;MainWindow::updateQueueStatus);
    statusTimer-&gt;start(1000);
    
<<<<<<< HEAD
    // Thread slider setup
    threadSlider = new QSlider(Qt::Horizontal, this);
    threadSlider-&gt;setRange(1, 32);
    threadSlider-&gt;setValue(parallelMgr-&gt;maxThreads());
    connect(threadSlider, &amp;QSlider::valueChanged, this, &amp;MainWindow::onThreadCountChanged);
    ui-&gt;statusBar()-&gt;addPermanentWidget(threadSlider);
    ui-&gt;statusBar()-&gt;addWidget(new QLabel(&quot;Threads: &quot;));
    ui-&gt;statusBar()-&gt;addWidget(new QLabel(&quot;1&quot;)); // Placeholder, update dynamically
    
    connect(driveMon, &amp;DriveMonitor::driveDisconnected, parallelMgr, [this]() { parallelMgr-&gt;pause(); });
    connect(driveMon, &amp;DriveMonitor::driveConnected, parallelMgr, &amp;ParallelManager::resume);
=======
    connect(driveMon, &amp;DriveMonitor::driveDisconnected, queueMgr, &amp;QueueManager::pauseAll);
    connect(driveMon, &amp;DriveMonitor::driveConnected, queueMgr, &amp;QueueManager::resumeAll);
    connect(driveMon, &amp;DriveMonitor::drivesChanged, this, [this](const QStringList &amp;drives){
        ui-&gt;statusBar()-&gt;showMessage(QString("Drives: %1").arg(drives.join(", ")), 2000);
    });
    connect(errorMgr, &amp;ErrorManager::errorOccurred, this, &amp;MainWindow::onErrorOccurred);
}

void MainWindow::onErrorOccurred(ErrorCategory cat, const QString&amp; message) {
    errorMgr-&gt;showUserDialog(cat, message);
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
    connect(ui-&gt;addTaskButton, &amp;QPushButton::clicked, this, &amp;MainWindow::on_addTaskButton_clicked);
    
    // Preview integration with QueueManager and ParallelManager
    connect(queueMgr, &amp;QueueManager::taskStatusChanged, this, [this](TransferTask*, TaskStatus) {
        previewScene-&gt;setQueueInfo(queueMgr-&gt;getTasks().size(), parallelMgr-&gt;activeThreads());
    });
    connect(parallelMgr, &amp;ParallelManager::progressUpdated, previewScene, &amp;PreviewScene::updateSpeedGraph);
    // Assume ParallelManager emits progressUpdated(double speed)
    // Similar for chunk flow, ETA
    
    // Drag drop for MainWindow and children
    setAcceptDrops(true);
    
    // Connect ParallelManager signals
    connect(parallelMgr, &amp;ParallelManager::taskStarted, this, [](TransferTask* t) {
        qDebug() &lt;&lt; &quot;Task started:&quot; &lt;&lt; t;
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
            // task-&gt;setPreset(presets); assume method
            queueMgr-&gt;addTask(task);
        }
        updateQueueStatus();
    }
}

void MainWindow::onThreadCountChanged(int value) {
    parallelMgr-&gt;setMaxThreads(value);
    queueMgr-&gt;setMaxParallel(value);
    qDebug() &lt;&lt; &quot;Threads set to&quot; &lt;&lt; value;
}

void MainWindow::updateQueueStatus() {
    ui-&gt;queueList-&gt;clear();
    QList&lt;TransferTask*&gt; tasks = queueMgr-&gt;getTasks();
    for (TransferTask *task : tasks) {
        QString itemText = QString(&quot;%1 -&gt; %2&quot;).arg(task-&gt;source().split(&#39;/&#39;).last(), task-&gt;targets().join(&quot;,&quot;));
        ui-&gt;queueList-&gt;addItem(itemText);
    }
    int active = parallelMgr-&gt;activeThreads();
    ui-&gt;statusBar()-&gt;showMessage(QString(&quot;Active threads: %1 | Queue: %2&quot;).arg(active).arg(tasks.size()));
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
    if (event-&gt;mimeData()-&gt;hasUrls()) {
        event-&gt;acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    QList&lt;QUrl&gt; urls = event-&gt;mimeData()-&gt;urls();
    QStringList sources;
    for (const QUrl&amp; url : urls) {
        QString path = url.toLocalFile();
        if (!path.isEmpty()) sources &lt;&lt; path;
    }
    if (!sources.isEmpty()) {
        AddTaskDialog dialog(this);
        dialog.setSources(sources); // Assume setSources method added
        if (dialog.exec() == QDialog::Accepted) {
            // Add as above
            for (const QString&amp; src : sources) {
                TransferTask *task = new TransferTask(src, dialog.getTargets());
                queueMgr-&gt;addTask(task);
            }
            updateQueueStatus();
        }
    }
    event-&gt;acceptProposedAction();
=======
void MainWindow::onParallelChanged(int value) {
    Settings::parallel() = value;
    queueMgr->setMaxParallel(value);
>>>>>>> origin/main
}