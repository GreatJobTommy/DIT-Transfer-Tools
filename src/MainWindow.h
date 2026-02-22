/****************************************************************************
** MainWindow.h - GUI-Hauptfenster für DIT-Transfer-Tools V3 Parallel-V2
** ParallelManager with QThreadPool 1-32, Priority Queue, Load Balance.
** Thread Slider in Statusbar.
****************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QTimer>
#include <QSlider>
#include "ParallelManager.h"
#include "AddTaskDialog.h"
#include "QueueManager.h"
#include "PreviewScene.h"
#include "DriveMonitor.h"
#include "HashManager.h"
#include "ProgressTracker.h"
#include <QProgressBar>
#include <QLabel>
#include <QWidget>
#include "QueueManager.h"
#include "DriveMonitor.h"
#include "HashManager.h"
#include "ProgressTracker.h"
#include <QProgressBar>
#include <QLabel>
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
    void on_addTaskButton_clicked();
    void onDropEvent(QDropEvent *event);
    void onTaskProgress(double perc, double speed, QString eta);
    void onTaskFinished(bool success);
    void onVerification(bool ok);
    void onProgressUpdated(double percentage, double mbPerSec, QString eta);
    void loadSettings();
    void saveSettings();
    void onThreadCountChanged(int value);
    void onParallelChanged(int value);
    void loadSettings();
    void saveSettings();
    void onErrorOccurred(ErrorCategory cat, const QString& message);
private:
    Ui::MainWindow *ui;
    ParallelManager *parallelMgr;
    QueueManager *queueMgr;
    DriveMonitor *driveMon;
    HashManager *hashMgr;
    ErrorManager *errorMgr;
    QTimer *statusTimer;
    QProgressBar *progressBar;
    QLabel *speedLabel;
    QLabel *etaLabel;
    QSlider *threadSlider;
    PreviewScene *previewScene;
    PreviewView *previewView;
    QWidget *previewWidget;
    
    void setupUI();
    void setupConnections();
};
#endif // MAINWINDOW_H
