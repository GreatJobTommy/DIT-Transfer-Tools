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
#include "QueueManager.h"
#include "AddTaskDialog.h"
#include "DriveMonitor.h"
#include "ProgressMonitor.h"
#include "ErrorManager.h"
#include "SettingsManager.h"
#include "DragDropList.h"

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

private:
    void setupUI();
    void setupHotkeys();
    void createDashboardTab();
    void createQueueTab();
    void createDrivesTab();
    void createSettingsTab();
    void createProgressTab();

    QueueManager* m_queue;
    DriveMonitor* m_driveMonitor;
    ProgressMonitor* m_progressMonitor;
    ErrorManager* m_errorManager;
    SettingsManager* m_settingsManager;

    QTabWidget* m_tabWidget;

    // Dashboard
    QLabel* m_queueCountLabel;
    QLabel* m_drivesCountLabel;
    QLabel* m_progressLabel;
    QLabel* m_errorsCountLabel;

    // Queue
    DragDropList* m_waitingList;
    DragDropList* m_dropZone;

    // Drives
    QListWidget* m_drivesList;

    // Settings

    // Progress
    QProgressBar* m_overallProgress;
    QTextEdit* m_logView;

    QAction* m_addTaskAction;
};

#endif // MAINWINDOW_H