#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "src/MainWindow.h"
#include "src/QueueManager.h"

// Transfer App main.cpp - Drive/Ordner/Multi Features from DIT-Transfer-Tools
// Full implementation in DIT-Transfer-Tools repo
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("DIT-Transfer-Tools");
    app.setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("DIT Transfer Tools");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption queueSizeOption(QStringList() << "queue-size", "Set maximum active transfers.", "size", "1");
    parser.addOption(queueSizeOption);

    parser.process(app);

    int queueSize = parser.value(queueSizeOption).toInt();
    if (queueSize <= 0) queueSize = 1;

    QueueManager* queue = new QueueManager(queueSize, &app);

    MainWindow w(queue);
    w.show();

    return app.exec();
}