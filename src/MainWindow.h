#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QProgressBar>
#include <QTextEdit>
#include <QSlider>
#include <QLabel>
#include <QAction>
#include <QSystemTrayIcon>
#include "QueueManager.h"
#include "AddTaskDialog.h"
#include "DriveMonitor.h"
#include "ProgressMonitor.h"
#include "ErrorManager.h"
#include "SettingsManager.h"
#include "SpeedHistory.h"
#include "DragDropList.h"
#include "ErrorManager.h"
#include "SettingsManager.h"

#include "LogsDockWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QueueManager* queue, QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void updateLists();
    void addTask();
    void onFilesDropped(const QStringList& files);
    void ejectDrive();
    void updateDashboard();
    void updateDrives();
    void updateProgress();
    void updateErrors();
    void settingChanged(const QString& key, const QVariant& value);
    void onTaskCompleted(TransferTask* task);
    void onTaskFailed(TransferTask* task);
    void onTaskPaused(TransferTask* task);
    void onDriveReconnected();
    void onProgressChanged(qint64 bytes, qint64 speed, qint64 eta);

private:
    void setupUI();
    void setupHotkeys();

    QueueManager* m_queue;
    DriveMonitor* m_driveMonitor;
    ProgressMonitor* m_progressMonitor;
    ErrorManager* m_errorManager;
    SettingsManager* m_settingsManager;
    SpeedHistory* m_speedHistory;

    QTabWidget* m_tabWidget;
    QListWidget* m_drivesList;
    QProgressBar* m_overallProgress;
    QTextEdit* m_logView;
    DragDropList* m_dropZone;

    LogsDockWidget* m_logsDock;

    // Dashboard
    QLabel* m_queueCountLabel;
    QLabel* m_drivesCountLabel;
    QLabel* m_progressLabel;
    QLabel* m_errorsCountLabel;

    // Queue
    QListWidget* m_waitingList;

    // Drives
    QListWidget* m_drivesList;

    // Settings
    QSlider* m_parallelSlider;
    QLabel* m_parallelLabel;

    // Progress (now in dashboard)
    QProgressBar* m_overallProgress;

    QAction* m_addTaskAction;
    QSystemTrayIcon* m_trayIcon;
};

#endif // MAINWINDOW_H