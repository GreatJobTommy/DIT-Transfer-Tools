<<<<<<< HEAD
void MainWindow::onProgressUpdated(double percentage, double mbPerSec, QString eta)
{
    progressBar-&gt;setValue(static_cast&lt;int&gt;(percentage));
    speedLabel-&gt;setText(QString(&quot;Geschw.: %1 MB/s&quot;).arg(mbPerSec, 0, &#x27;f&#x27;, 1));
    etaLabel-&gt;setText(QString(&quot;ETA: %1&quot;).arg(eta));
}
=======
/****************************************************************************
** MainWindow.cpp - GUI Implementation V3 (DriveMonitor Integration)
** Features: Queue-View, Settings-Dialog, Status-Updates, Drive Events.
****************************************************************************/

#include &quot;MainWindow.h&quot;
#include &quot;ui_MainWindow.h&quot;
#include &lt;QFileDialog&gt;
#include &lt;QSettings&gt;
#include &lt;QDebug&gt;
#include &lt;QStandardPaths&gt;

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
    
    // CRITICAL: DriveMonitor &lt;-&gt; QueueManager signals
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

void MainWindow::setupUI()
{
    setWindowTitle(&quot;DIT-Transfer-Tools V3&quot;);
    resize(900, 700);
    ui-&gt;statusBar()-&gt;showMessage(&quot;DriveMonitor V3 active&quot;);
}

void MainWindow::setupConnections()
{
    connect(ui-&gt;addTaskButton, &amp;QPushButton::clicked, [this](){
        QString src = QFileDialog::getOpenFileName(this, &quot;Quelle wählen&quot;);
        QString dst = QFileDialog::getSaveFileName(this, &quot;Ziel Drive wählen&quot;);
        if (!src.isEmpty() &amp;&amp; !dst.isEmpty()) {
            TransferTask *task = new TransferTask(src, dst, this);
            queueMgr-&gt;addTask(task);
        }
    });
    
    // Resume button
    connect(ui-&gt;resumeButton, &amp;QPushButton::clicked, queueMgr, &amp;QueueManager::resumeAll);
}

void MainWindow::updateQueueStatus()
{
    // Update queue list in UI
    int active = queueMgr-&gt;getTasks().count([](TransferTask *t){ return t-&gt;status() == TaskStatus::Active; });
    ui-&gt;statusLabel-&gt;setText(QString(&quot;Active: %1 | Drives: %2&quot;)
        .arg(active).arg(driveMon-&gt;getCurrentDrives().size()));
}

void MainWindow::onDriveReconnect(const QString &amp;drive)
{
    ui-&gt;statusBar()-&gt;showMessage(&quot;✅ Drive reconnected: &quot; + drive, 5000);
}

void MainWindow::loadSettings()
{
    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + &quot;/dit.ini&quot;, QSettings::IniFormat);
    queueMgr-&gt;setMaxParallel(settings.value(&quot;queue/maxParallel&quot;, 4).toInt());
}

void MainWindow::saveSettings()
{
    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + &quot;/dit.ini&quot;, QSettings::IniFormat);
    settings.setValue(&quot;queue/maxParallel&quot;, queueMgr-&gt;getMaxParallel());
}

// EOF
>>>>>>> origin/main
