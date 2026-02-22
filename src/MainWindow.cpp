#include &quot;MainWindow.h&quot;
#include &quot;ui_MainWindow.h&quot;
#include &quot;ParallelManager.h&quot;
#include &lt;QFileDialog&gt;
#include &lt;QSettings&gt;
#include &lt;QDebug&gt;
#include &lt;QSlider&gt;
#include &lt;QVBoxLayout&gt;
#include &lt;QLabel&gt;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui-&gt;setupUi(this);
    setupUI();
    
    parallelMgr = new ParallelManager(this);
    driveMon = new DriveMonitor(this);
    hashMgr = new HashManager(this);
    
    setupConnections();
    loadSettings();
    
    statusTimer = new QTimer(this);
    connect(statusTimer, &amp;QTimer::timeout, this, &amp;MainWindow::updateQueueStatus);
    statusTimer-&gt;start(1000);
    
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
    // Connect ParallelManager signals
    connect(parallelMgr, &amp;ParallelManager::taskStarted, this, [](TransferTask* t) {
        qDebug() &lt;&lt; &quot;Task started:&quot; &lt;&lt; t;
    });
    connect(parallelMgr, &amp;ParallelManager::taskFinished, this, &amp;MainWindow::onTaskFinished);
}

void MainWindow::onThreadCountChanged(int value) {
    parallelMgr-&gt;setMaxThreads(value);
    qDebug() &lt;&lt; &quot;Threads set to&quot; &lt;&lt; value;
    // Update label if added
}

void MainWindow::updateQueueStatus() {
    // Update with parallelMgr stats
    int active = parallelMgr-&gt;activeThreads();
    ui-&gt;statusBar()-&gt;showMessage(QString(&quot;Active threads: %1&quot;).arg(active));
}

// Placeholder slots
void MainWindow::onTaskProgress(double, double, QString) {}
void MainWindow::onTaskFinished(bool) {}
void MainWindow::onVerification(bool) {}
void MainWindow::onProgressUpdated(double, double, QString) {}
void MainWindow::loadSettings() {}
void MainWindow::saveSettings() {}
