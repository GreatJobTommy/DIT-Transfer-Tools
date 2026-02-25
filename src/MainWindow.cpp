#include "MainWindow.h"
#include "TransferTask.h"
#include <QLabel>

MainWindow::MainWindow(QueueManager* queue, QWidget* parent)
    : QMainWindow(parent), m_queue(queue) {
    setWindowTitle("DIT Transfer Tools");

    QWidget* central = new QWidget;
    setCentralWidget(central);

    QHBoxLayout* mainLayout = new QHBoxLayout(central);

    // Active list
    QVBoxLayout* activeLayout = new QVBoxLayout;
    activeLayout->addWidget(new QLabel("Active Transfers"));
    m_activeList = new QListWidget;
    activeLayout->addWidget(m_activeList);

    // Waiting list
    QVBoxLayout* waitingLayout = new QVBoxLayout;
    waitingLayout->addWidget(new QLabel("Waiting Transfers"));
    m_waitingList = new QListWidget;
    waitingLayout->addWidget(m_waitingList);

    // Buttons
    QVBoxLayout* buttonLayout = new QVBoxLayout;
    m_addTaskBtn = new QPushButton("Add Task");
    m_reorderUpBtn = new QPushButton("Move Up");
    m_reorderDownBtn = new QPushButton("Move Down");
    buttonLayout->addWidget(m_addTaskBtn);
    buttonLayout->addWidget(m_reorderUpBtn);
    buttonLayout->addWidget(m_reorderDownBtn);
    buttonLayout->addStretch();

    waitingLayout->addLayout(buttonLayout);

    mainLayout->addLayout(activeLayout);
    mainLayout->addLayout(waitingLayout);

    connect(m_addTaskBtn, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(m_reorderUpBtn, &QPushButton::clicked, this, &MainWindow::reorderUp);
    connect(m_reorderDownBtn, &QPushButton::clicked, this, &MainWindow::reorderDown);

    // For demo, add some tasks
    TransferTask* t1 = new TransferTask("src1", "dst1");
    TransferTask* t2 = new TransferTask("src2", "dst2");
    m_queue->addTask(t1);
    m_queue->addTask(t2);

    updateLists();
}

MainWindow::~MainWindow() {}

void MainWindow::updateLists() {
    m_activeList->clear();
    for (auto task : m_queue->activeTasks()) {
        m_activeList->addItem(task->source() + " -> " + task->destination());
    }

    m_waitingList->clear();
    for (auto task : m_queue->waitingTasks()) {
        m_waitingList->addItem(task->source() + " -> " + task->destination());
    }
}

void MainWindow::reorderUp() {
    int row = m_waitingList->currentRow();
    if (row > 0) {
        m_queue->reorderWaiting(row, row - 1);
        updateLists();
        m_waitingList->setCurrentRow(row - 1);
    }
}

void MainWindow::reorderDown() {
    int row = m_waitingList->currentRow();
    if (row >= 0 && row < m_waitingList->count() - 1) {
        m_queue->reorderWaiting(row, row + 1);
        updateLists();
        m_waitingList->setCurrentRow(row + 1);
    }
}

void MainWindow::addTask() {
    AddTaskDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        TransferTask* task = dialog.getTask();
        m_queue->addTask(task);
        updateLists();
    }
}