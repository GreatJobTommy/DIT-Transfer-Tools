#include "MainWindow.h"
#include "ui_MainWindow.h"
#include &lt;QFileDialog&gt;
#include &lt;QSettings&gt;
#include &lt;QDebug&gt;
#include &lt;QStandardPaths&gt;
#include &lt;QFileInfo&gt;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui-&gt;setupUi(this);
    setupUI();
    
    queueMgr = new QueueManager(this);
    driveMon = new DriveMonitor(this);
    hashMgr = new HashManager(this);
    errorMgr = new ErrorManager(this);
    
    setupConnections();
    loadSettings();
    
    statusTimer = new QTimer(this);
    connect(statusTimer, &amp;QTimer::timeout, this, &amp;MainWindow::updateQueueStatus);
    statusTimer-&gt;start(1000);
    
    connect(driveMon, &amp;DriveMonitor::driveDisconnected, queueMgr, &amp;QueueManager::pauseAll);
    connect(driveMon, &amp;DriveMonitor::driveConnected, queueMgr, &amp;QueueManager::resumeAll);
    connect(driveMon, &amp;DriveMonitor::drivesChanged, this, [this](const QStringList &amp;drives){
        ui-&gt;statusBar()-&gt;showMessage(QString("Drives: %1").arg(drives.join(", ")), 2000);
    });
    connect(errorMgr, &amp;ErrorManager::errorOccurred, this, &amp;MainWindow::onErrorOccurred);
}

void MainWindow::onErrorOccurred(ErrorCategory cat, const QString&amp; message) {
    errorMgr-&gt;showUserDialog(cat, message);
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::onParallelChanged(int value) {
    Settings::parallel() = value;
    queueMgr->setMaxParallel(value);
}