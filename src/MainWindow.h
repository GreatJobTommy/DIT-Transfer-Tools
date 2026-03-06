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
#include "SettingsManager.h"
#include "DragDropList.h"
#include "LogsDockWidget.h"
#include <QDateTime>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QueueManager* queue, QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void updateLists();
    void addTask();
    void addMultiTask();
    void addMultiTasks(const QList<TransferTask*>& tasks);
    void startQueue();
    void runSpeedtest();
    void showQueueContextMenu(const QPoint& pos);
    void showDrivesContextMenu(const QPoint& pos);
    void changeTheme(const QString& theme);
    void onFilesDropped(const QStringList& files);
    void ejectDrive();
    void updateProgress();
    void settingChanged(const QString& key, const QVariant& value);
    void onTaskCompleted(TransferTask* task);
    void onTaskFailed(TransferTask* task);
    void onTaskPaused(TransferTask* task);
    void onDriveReconnected();
    void onProgressChanged(qint64 bytes, qint64 speed, qint64 eta);
void createRcloneTab();

void refreshRcloneRemotes();
void addRcloneRemote();
void editRcloneRemote();
void deleteRcloneRemote();

private:
    void setupUI();
    void setupHotkeys();
    void createDashboardTab();
    void createQueueTab();
    void createDrivesTab();
    void createNodeTab();
    void createSettingsTab();

    QueueManager* m_queue;
    DriveMonitor* m_driveMonitor;
    ProgressMonitor* m_progressMonitor;
    SettingsManager* m_settingsManager;

    QTabWidget* m_tabWidget;

    // Dashboard
    QLabel* m_queueCountLabel;
    QLabel* m_drivesCountLabel;
    QLabel* m_progressLabel;
    QLabel* m_errorsCountLabel;

    // Queue
    DragDropList* m_waitingList;

    // Drives
    QListWidget* m_drivesList;

    // Settings
    QSlider* m_parallelSlider;
    QLabel* m_parallelLabel;

    // Progress
    QProgressBar* m_overallProgress;

    QAction* m_addTaskAction;
    QAction* m_removeTaskAction;
    QSystemTrayIcon* m_trayIcon;

// Rclone
QListWidget* m_rcloneRemotesList;
QPushButton* m_refreshRcloneBtn;
QPushButton* m_addRcloneBtn;
QPushButton* m_editRcloneBtn;
QPushButton* m_deleteRcloneBtn;
};

#endif // MAINWINDOW_H