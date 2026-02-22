#include &quot;SettingsManager.h&quot;
#include &lt;QStandardPaths&gt;
#include &lt;QDir&gt;
#include &lt;QCoreApplication&gt;
#include &lt;QCryptographicHash&gt;

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , m_settings(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + QDir::separator() + &quot;settings.ini&quot;, QSettings::IniFormat)
{
    // Defaults
    if (!m_settings.contains(&quot;parallelTasks&quot;)) {
        m_settings.setValue(&quot;parallelTasks&quot;, 4);
    }
    if (!m_settings.contains(&quot;hashAlgo&quot;)) {
        m_settings.setValue(&quot;hashAlgo&quot;, &quot;SHA256&quot;);
    }
    if (!m_settings.contains(&quot;drivePollInterval&quot;)) {
        m_settings.setValue(&quot;drivePollInterval&quot;, 10);
    }
    m_settings.sync();
}

int SettingsManager::parallelTasks() const {
    return m_settings.value(&quot;parallelTasks&quot;, 4).toInt();
}

QString SettingsManager::hashAlgo() const {
    return m_settings.value(&quot;hashAlgo&quot;, &quot;SHA256&quot;).toString();
}

int SettingsManager::drivePollInterval() const {
    return m_settings.value(&quot;drivePollInterval&quot;, 10).toInt();
}

void SettingsManager::setParallelTasks(int value) {
    if (value &gt;= 1 &amp;&amp; value &lt;= 32) {
        m_settings.setValue(&quot;parallelTasks&quot;, value);
        m_settings.sync();
        emitChanged();
    }
}

void SettingsManager::setHashAlgo(const QString&amp; algo) {
    if (algo == &quot;SHA256&quot; || algo == &quot;SHA512&quot;) {
        m_settings.setValue(&quot;hashAlgo&quot;, algo);
        m_settings.sync();
        emitChanged();
    }
}

void SettingsManager::setDrivePollInterval(int seconds) {
    if (seconds &gt;= 1 &amp;&amp; seconds &lt;= 60) {
        m_settings.setValue(&quot;drivePollInterval&quot;, seconds);
        m_settings.sync();
        emitChanged();
    }
}

void SettingsManager::emitChanged() {
    emit settingsChanged();
}

QByteArray SettingsManager::serialize() const {
    return m_settings.serialize(); // Note: QSettings doesn't have direct serialize, this is simplified
}

bool SettingsManager::deserialize(const QByteArray&amp; data) {
    // Simplified, would need proper QSettings deserialization
    return false;
}
