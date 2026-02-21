/****************************************************************************
** MainWindow.cpp - GUI Implementation
** Features: Queue-View, Settings-Dialog, Status-Updates, Log-Viewer.
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
    setupConnections();
    loadSettings();
    
    queueMgr = new QueueManager(this);
    driveMon = new DriveMonitor(this);
    hashMgr = new HashManager(this);
    
    statusTimer = new QTimer(this);
    connect(statusTimer, &amp;QTimer::timeout, this, &amp;MainWindow::updateQueueStatus);
    statusTimer-&gt;start(1000); // 1s Update
    
    connect(driveMon, &amp;DriveMonitor::driveReconnected, this, &amp;MainWindow::onDriveReconnect);
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::setupUI()
{
    setWindowTitle(&quot;DIT-Transfer-Tools&quot;);
    resize(800, 600);
}

void MainWindow::setupConnections()
{
    connect(ui-&gt;addTaskButton, &amp;QPushButton::clicked, [this](){
        QString src = QFileDialog::getOpenFileName(this, &quot;Quelle wählen&quot;);
        QString dst = QFileDialog::getSaveFileName(this, &quot;Ziel wählen&quot;);
        if (!src.isEmpty() &amp;&amp; !dst.isEmpty()) {
            queueMgr-&gt;addTask(src, dst);
        }
    });
}

void MainWindow::updateQueueStatus()
{
    // UI mit Queue-Status aktualisieren (aktiv/wartend)
    // TODO: Liste refreshen
}

void MainWindow::onDriveReconnect(const QString &amp;drive)
{
    ui-&gt;statusBar()-&gt;showMessage(&quot;Drive reconnected: &quot; + drive, 3000);
}

void MainWindow::loadSettings()
{
    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + &quot;/settings.ini&quot;, QSettings::IniFormat);
    // Chunk-Größe, Hash-Enable etc.
}

void MainWindow::saveSettings()
{
    // Settings speichern
}

// EOF