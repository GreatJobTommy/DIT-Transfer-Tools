#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include &lt;QMainWindow&gt;
#include &lt;QTimer&gt;
#include "QueueManager.h"
#include "DriveMonitor.h"
#include "HashManager.h"
#include "ProgressTracker.h"
#include &lt;QProgressBar&gt;
#include &lt;QLabel&gt;
#include "ErrorManager.h"

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
    void onProgressUpdated(double percentage, double mbPerSec, QString eta);
    void onParallelChanged(int value);
    void loadSettings();
    void saveSettings();
    void onErrorOccurred(ErrorCategory cat, const QString&amp; message);

private:
    Ui::MainWindow *ui;
    QueueManager *queueMgr;
    DriveMonitor *driveMon;
    HashManager *hashMgr;
    ErrorManager *errorMgr;
    QTimer *statusTimer;
    QProgressBar *progressBar;
    QLabel *speedLabel;
    QLabel *etaLabel;
    
    void setupUI();
    void setupConnections();
};

#endif // MAINWINDOW_H