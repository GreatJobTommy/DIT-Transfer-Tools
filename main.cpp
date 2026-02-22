/****************************************************************************
** DIT-Transfer-Tools v1.0 - Haupt-Einstiegspunkt
** Modularer Dateitransfer mit Hashing, Queue und Drive-Monitoring.
** Autor: OpenClaw Builder | Erstellt: 2026
****************************************************************************/

#include <QApplication>
#include <QDir>
#include "src/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Log-Verzeichnis initialisieren
    QDir logDir(QApplication::applicationDirPath());
    if (!logDir.exists("logs")) {
        logDir.mkdir("logs");
    }
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

// EOF