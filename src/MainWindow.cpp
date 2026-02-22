#include &quot;MainWindow.h&quot;
#include &quot;ui_MainWindow.h&quot;
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
    
    setupConnections();
    loadSettings();
    
    statusTimer = new QTimer(this);
    connect(statusTimer, &amp;QTimer::timeout, this, &amp;MainWindow::updateQueueStatus);
    statusTimer-&gt;start(1000);
    
    connect(driveMon, &amp;DriveMonitor::driveDisconnected, queueMgr, &amp;QueueManager::pauseAll);
    connect(driveMon, &amp;DriveMonitor::driveConnected, queueMgr, &amp;QueueManager::resumeAll);
    connect(driveMon, &amp;DriveMonitor::drivesChanged, this, [this](const QStringList &amp;drives){
        ui-&gt;statusBar()-&gt;showMessage(QString(&quot;Drives: %1&quot;).arg(drives.join(&quot;, &quot;)), 2000);
    });
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