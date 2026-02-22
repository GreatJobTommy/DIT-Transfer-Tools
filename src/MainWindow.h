/****************************************************************************
** MainWindow.h - GUI-Hauptfenster für DIT-Transfer-Tools V3 Parallel-V2
** ParallelManager with QThreadPool 1-32, Priority Queue, Load Balance.
** Thread Slider in Statusbar.
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include &lt;QMainWindow&gt;
#include &lt;QTimer&gt;
#include &lt;QSlider&gt;
#include &quot;ParallelManager.h&quot;
#include &quot;AddTaskDialog.h&quot;
#include &quot;QueueManager.h&quot;
#include &quot;DriveMonitor.h&quot;
#include &quot;HashManager.h&quot;
#include &quot;ProgressTracker.h&quot;
#include &lt;QProgressBar&gt;
#include &lt;QLabel&gt;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateQueueStatus();
    void on_addTaskButton_clicked();
    void onDropEvent(QDropEvent *event);
    void onTaskProgress(double perc, double speed, QString eta);
    void onTaskFinished(bool success);
    void onVerification(bool ok);
    void onProgressUpdated(double percentage, double mbPerSec, QString eta);
    void loadSettings();
    void saveSettings();
    void onThreadCountChanged(int value);

private:
    Ui::MainWindow *ui;
    ParallelManager *parallelMgr;
    QueueManager *queueMgr;
    DriveMonitor *driveMon;
    HashManager *hashMgr;
    QTimer *statusTimer;
    QProgressBar *progressBar;
    QLabel *speedLabel;
    QLabel *etaLabel;
    QSlider *threadSlider;
    
    void setupUI();
    void setupConnections();
};

#endif // MAINWINDOW_H
