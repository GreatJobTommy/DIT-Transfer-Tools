#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "QueueManager.h"
#include "AddTaskDialog.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QueueManager* queue, QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void updateLists();
    void reorderUp();
    void reorderDown();
    void addTask();

private:
    QueueManager* m_queue;
    QListWidget* m_activeList;
    QListWidget* m_waitingList;
    QPushButton* m_addTaskBtn;
    QPushButton* m_reorderUpBtn;
    QPushButton* m_reorderDownBtn;
};

#endif // MAINWINDOW_H