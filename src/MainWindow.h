/****************************************************************************
** MainWindow.h - GUI-Hauptfenster für DIT-Transfer-Tools
** Steuert Queue-Status, Settings, Logs und Drive-Monitor.
** Max 400 Zeilen.
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include &lt;QMainWindow&gt;
#include &lt;QTimer&gt;
#include &quot;QueueManager.h&quot;
#include &quot;DriveMonitor.h&quot;
#include &quot;HashManager.h&quot;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateQueueStatus();
    void onDriveReconnect(const QString &amp;drive);
    void loadSettings();
    void saveSettings();

private:
    Ui::MainWindow *ui;
    QueueManager *queueMgr;
    DriveMonitor *driveMon;
    HashManager *hashMgr;
    QTimer *statusTimer;
    
    void setupUI();
    void setupConnections();
};

#endif // MAINWINDOW_H