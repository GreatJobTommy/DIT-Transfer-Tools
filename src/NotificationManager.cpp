#include "NotificationManager.h"
#include <QMessageBox>
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QOperatingSystemVersion>

NotificationManager::NotificationManager(QObject* parent)
    : QObject(parent),
      m_trayIcon(nullptr),
      m_notifyProcess(new QProcess(this)),
      m_soundTimer(new QTimer(this)),
      m_notificationsEnabled(true),
      m_soundEnabled(true),
      m_useSystemNotify(false) {

    m_soundTimer->setSingleShot(true);
    // Timer not used without sound support

    // Default sound file - try system default notification sound
    QString defaultSound;
#if defined(Q_OS_LINUX)
    defaultSound = "/usr/share/sounds/freedesktop/stereo/complete.oga";
    if (!QFile::exists(defaultSound)) {
        defaultSound = "/usr/share/sounds/gnome/default/alerts/glass.ogg";
    }
    if (!QFile::exists(defaultSound)) {
        defaultSound = ""; // No default found
    }
#elif defined(Q_OS_WIN)
    defaultSound = ""; // Windows has default system sounds
#elif defined(Q_OS_MAC)
    defaultSound = "/System/Library/Sounds/Glass.aiff";
#endif
    m_soundFile = defaultSound;
}

NotificationManager::~NotificationManager() {}

void NotificationManager::setTrayIcon(QSystemTrayIcon* trayIcon) {
    m_trayIcon = trayIcon;
    if (m_trayIcon) {
        connect(m_trayIcon, &QSystemTrayIcon::messageClicked, this, &NotificationManager::onTrayMessageClicked);
    }
}

void NotificationManager::showNotification(const QString& title, const QString& message,
                                         QSystemTrayIcon::MessageIcon icon) {
    if (!m_notificationsEnabled) return;

    if (m_useSystemNotify) {
        showSystemNotification(title, message);
    } else if (m_trayIcon && m_trayIcon->isVisible()) {
        m_trayIcon->showMessage(title, message, icon, 5000); // 5 seconds
    } else {
        showFallbackNotification(title, message);
    }

    // Play sound if enabled
    if (m_soundEnabled) {
        playSound(m_soundFile);
    }
}

void NotificationManager::playSound(const QString& soundFile) {
    Q_UNUSED(soundFile)
    // For now, just use system beep since multimedia is not available
    QApplication::beep();
}

void NotificationManager::showSystemNotification(const QString& title, const QString& message) {
    QString program;
    QStringList arguments;

#if defined(Q_OS_LINUX)
    // Try notify-send first (libnotify)
    if (QStandardPaths::findExecutable("notify-send").isEmpty() == false) {
        program = "notify-send";
        arguments << title << message << "--icon=dialog-information";
    } else {
        // Fallback to DBUS
        program = "dbus-send";
        arguments << "--session"
                  << "--dest=org.freedesktop.Notifications"
                  << "--type=method_call"
                  << "--print-reply"
                  << "/org/freedesktop/Notifications"
                  << "org.freedesktop.Notifications.Notify"
                  << "string:DIT-Transfer-Tools"
                  << "uint32:0"
                  << "string:"
                  << "string:" + title
                  << "string:" + message
                  << "array:string:"
                  << "dict:string:string::"
                  << "int32:5000";
    }
#elif defined(Q_OS_WIN)
    // Windows notification - for now use fallback
    showFallbackNotification(title, message);
    return;
#elif defined(Q_OS_MAC)
    // macOS notification
    program = "osascript";
    arguments << "-e" << QString("display notification \"%1\" with title \"%2\"").arg(message, title);
#endif

    if (!program.isEmpty()) {
        m_notifyProcess->start(program, arguments);
    } else {
        showFallbackNotification(title, message);
    }
}

void NotificationManager::showFallbackNotification(const QString& title, const QString& message) {
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void NotificationManager::onTrayMessageClicked() {
    // Could emit a signal or bring window to front
    // For now, just debug
    qDebug() << "Notification clicked";
}

// Settings methods
void NotificationManager::setNotificationsEnabled(bool enabled) {
    m_notificationsEnabled = enabled;
}

void NotificationManager::setSoundEnabled(bool enabled) {
    m_soundEnabled = enabled;
}

void NotificationManager::setSoundFile(const QString& file) {
    m_soundFile = file;
}

void NotificationManager::setUseSystemNotify(bool useSystem) {
    m_useSystemNotify = useSystem;
}

bool NotificationManager::notificationsEnabled() const {
    return m_notificationsEnabled;
}

bool NotificationManager::soundEnabled() const {
    return m_soundEnabled;
}

QString NotificationManager::soundFile() const {
    return m_soundFile;
}

bool NotificationManager::useSystemNotify() const {
    return m_useSystemNotify;
}