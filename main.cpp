/****************************************************************************
** DIT-Transfer-Tools v1.0 - Haupt-Einstiegspunkt
** Modularer Dateitransfer mit Hashing, Queue und Drive-Monitoring.
** Autor: OpenClaw Builder | Erstellt: 2026
****************************************************************************/

#include &lt;QApplication&gt;
#include &lt;QDir&gt;
#include &quot;src/MainWindow.h&quot;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Log-Verzeichnis initialisieren
    QDir logDir(QApplication::applicationDirPath());
    if (!logDir.exists(&quot;logs&quot;)) {
        logDir.mkdir(&quot;logs&quot;);
    }
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

// EOF