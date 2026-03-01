#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QProgressBar>
#include <QLabel>
#include <QTableWidget>
#include <QDockWidget>
#include <QSettings>
#include "QueueManager.h"
#include "AddTaskDialog.h"
#include "DriveMonitor.h"
#include "SettingsManager.h"
#include "ProgressMonitor.h"
#include "ErrorManager.h"
#include "ParallelManager.h"
#include "HashManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QueueManager* queue, QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void updateLists();
    void reorderUp();
    void reorderDown();
    void addTask();
    void onDriveAdded(const QStorageInfo& drive);
    void onDriveRemoved(const QStorageInfo& drive);
    void onProgressUpdated(double speed, qint64 eta, const QString& log);
    void onErrorAdded(const QString& error);

private:
    void setupUI();
    void setupTabs();
    void setupQueueTab();
    void setupDrivesTab();
    void setupAddTaskTab();
    void setupSettingsTab();
    void setupProgressTab();
    void saveWindowState();
    void restoreWindowState();

    QueueManager* m_queue;
    ParallelManager* m_parallel;
    DriveMonitor* m_driveMonitor;
    SettingsManager* m_settings;
    ProgressMonitor* m_progress;
    ErrorManager* m_errors;
    HashManager* m_hash;

    QTabWidget* m_tabWidget;

    // Queue tab
    QListWidget* m_activeList;
    QListWidget* m_waitingList;
    QPushButton* m_addTaskBtn;
    QPushButton* m_reorderUpBtn;
    QPushButton* m_reorderDownBtn;

    // Drives tab
    QTableWidget* m_drivesTable;

    // Add Task tab
    AddTaskDialog* m_addTaskDialog;

    // Settings tab
    QTextEdit* m_settingsEdit;

    // Progress tab
    QProgressBar* m_progressBar;
    QLabel* m_speedLabel;
    QLabel* m_etaLabel;
    QTextEdit* m_logEdit;

    // Dock
    QDockWidget* m_errorDock;
    QTextEdit* m_errorEdit;
};

#endif // MAINWINDOW_H