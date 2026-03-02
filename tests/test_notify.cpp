#include <QtTest>
#include <QSignalSpy>
#include "../src/NotificationManager.h"
#include "../src/SettingsManager.h"
#include <QSystemTrayIcon>

// add necessary includes here

class TestNotify : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testNotificationShow();
    void testSoundPlayback();
    void testSystemNotification();
    void testSettingsIntegration();

private:
    NotificationManager* m_notificationManager;
    SettingsManager* m_settingsManager;
    QSystemTrayIcon* m_trayIcon;
};

void TestNotify::initTestCase() {
    m_settingsManager = new SettingsManager();
    m_notificationManager = new NotificationManager();
    m_trayIcon = new QSystemTrayIcon();
    m_trayIcon->setIcon(QIcon(":/icons/app.png"));
    m_notificationManager->setTrayIcon(m_trayIcon);
}

void TestNotify::cleanupTestCase() {
    delete m_notificationManager;
    delete m_settingsManager;
    delete m_trayIcon;
}

void TestNotify::testNotificationShow() {
    // Test showing notification
    QSignalSpy spy(m_trayIcon, &QSystemTrayIcon::messageClicked);
    m_notificationManager->showNotification("Test Title", "Test Message");

    // Wait for potential tray message
    QTest::qWait(100);

    // Verify notification manager is configured
    QVERIFY(m_notificationManager->notificationsEnabled());
}

void TestNotify::testSoundPlayback() {
    // Test sound playback
    m_notificationManager->playSound();

    // Wait for sound to play
    QTest::qWait(500);

    QVERIFY(m_notificationManager->soundEnabled());
}

void TestNotify::testSystemNotification() {
    // Test system notification (if available)
    m_notificationManager->setUseSystemNotify(true);
    m_notificationManager->showNotification("System Test", "System notification test");

    QTest::qWait(100);

    QVERIFY(m_notificationManager->useSystemNotify());
}

void TestNotify::testSettingsIntegration() {
    // Test settings integration
    m_settingsManager->setNotificationsEnabled(false);
    QVERIFY(!m_settingsManager->getNotificationsEnabled());

    m_settingsManager->setNotificationSoundEnabled(false);
    QVERIFY(!m_settingsManager->getNotificationSoundEnabled());

    m_settingsManager->setNotificationSoundFile("/test/sound.wav");
    QCOMPARE(m_settingsManager->getNotificationSoundFile(), QString("/test/sound.wav"));

    m_settingsManager->setUseSystemNotifications(true);
    QVERIFY(m_settingsManager->getUseSystemNotifications());
}

QTEST_MAIN(TestNotify)
#include "test_notify.moc"