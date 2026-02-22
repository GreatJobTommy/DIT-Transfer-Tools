/****************************************************************************
** MainWindow.h - GUI-Hauptfenster für DIT-Transfer-Tools V3
** DriveMonitor fully integrated with signals to QueueManager.
** Max 400 Zeilen.
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include &lt;QMainWindow&gt;
#include &lt;QTimer&gt;
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
    void onTaskProgress(double perc, double speed, QString eta);
    void onTaskFinished(bool success);
    void onVerification(bool ok);
<<<<<<< HEAD
    void onProgressUpdated(double percentage, double mbPerSec, QString eta);
    void onParallelChanged(int value);
=======
>>>>>>> origin/main
    void loadSettings();
    void saveSettings();

private:
    Ui::MainWindow *ui;
    QueueManager *queueMgr;
    DriveMonitor *driveMon;
    HashManager *hashMgr;
    QTimer *statusTimer;
    QProgressBar *progressBar;
    QLabel *speedLabel;
    QLabel *etaLabel;
    
    void setupUI();
    void setupConnections();
};

#endif // MAINWINDOW_H