#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QProgressBar>
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0) : QMainWindow(parent) {
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

        QLabel *titleLabel = new QLabel("DIT Transfer Tool v1.0", this);
        titleLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
        mainLayout->addWidget(titleLabel);

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *addButton = new QPushButton("Add Task", this);
        buttonLayout->addWidget(addButton);
        QPushButton *clearButton = new QPushButton("Clear Queue", this);
        buttonLayout->addWidget(clearButton);
        mainLayout->addLayout(buttonLayout);

        queueList = new QListWidget(this);
        mainLayout->addWidget(queueList);

        progressBar = new QProgressBar(this);
        progressBar->setRange(0, 100);
        progressBar->setValue(0);
        mainLayout->addWidget(progressBar);

        connect(addButton, SIGNAL(clicked()), this, SLOT(addTask()));
        connect(clearButton, SIGNAL(clicked()), queueList, SLOT(clear()));

        setWindowTitle("DIT Transfer Tool v1.0");
        resize(600, 400);
    }

private slots:
    void addTask() {
        queueList->addItem("New Transfer Task");
        progressBar->setValue(0);
    }

private:
    QListWidget *queueList;
    QProgressBar *progressBar;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

#include "main.moc"
