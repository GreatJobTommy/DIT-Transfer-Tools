#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QProcess>
#include <QTimer>

class NotificationManager : public QObject {
    Q_OBJECT

public:
    explicit NotificationManager(QObject* parent = nullptr);
    ~NotificationManager();

    void setTrayIcon(QSystemTrayIcon* trayIcon);
    void showNotification(const QString& title, const QString& message,
                         QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information);
    void playSound(const QString& soundFile = QString());

    // Settings
    void setNotificationsEnabled(bool enabled);
    void setSoundEnabled(bool enabled);
    void setSoundFile(const QString& file);
    void setUseSystemNotify(bool useSystem);

    bool notificationsEnabled() const;
    bool soundEnabled() const;
    QString soundFile() const;
    bool useSystemNotify() const;

private slots:
    void onTrayMessageClicked();

private:
    void showSystemNotification(const QString& title, const QString& message);
    void showFallbackNotification(const QString& title, const QString& message);

    QSystemTrayIcon* m_trayIcon;
    QProcess* m_notifyProcess;
    QTimer* m_soundTimer;

    // Settings
    bool m_notificationsEnabled;
    bool m_soundEnabled;
    QString m_soundFile;
    bool m_useSystemNotify; // Use notify-send/DBUS instead of QSystemTrayIcon
};

#endif // NOTIFICATIONMANAGER_H