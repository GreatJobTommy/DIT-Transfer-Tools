#include &quot;ConfigManager.h&quot;
#include &lt;QStandardPaths&gt;
#include &lt;QDir&gt;
#include &lt;QJsonDocument&gt;
#include &lt;QFile&gt;
#include &lt;QDebug&gt;
#include &lt;QApplication&gt;

ConfigManager* ConfigManager::m_instance = nullptr;

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
    , m_qsettings(new QSettings(&quot;DIT-Transfer-Tools&quot;, &quot;Config&quot;, this))
    , m_watcher(new QFileSystemWatcher(this))
    , m_jsonBackupPath(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + &quot;/config.json&quot;)
{
    loadDefaults();
    migrateFromOldFormat();
    loadAllSettings();
    
    m_watcher-&gt;addPath(m_jsonBackupPath);
    connect(m_watcher, &amp;QFileSystemWatcher::fileChanged, this, &amp;ConfigManager::onJsonFileChanged);
}

ConfigManager::~ConfigManager() {
    saveAllSettings();
    m_instance = nullptr;
}

ConfigManager* ConfigManager::instance() {
    if (!m_instance) {
        m_instance = new ConfigManager();
    }
    return m_instance;
}

void ConfigManager::loadAllSettings() {
    loadFromJson(m_jsonBackupPath);
    m_qsettings-&gt;sync();
    emit settingsLoaded();
}

void ConfigManager::saveAllSettings() {
    saveToJson(m_jsonBackupPath);
    m_qsettings-&gt;sync();
    emit settingsSaved();
}

QString ConfigManager::getTransferDir() const {
    return m_qsettings-&gt;value(&quot;transfer/dir&quot;, QDir::homePath()).toString();
}

void ConfigManager::setTransferDir(const QString &amp;dir) {
    m_qsettings-&gt;setValue(&quot;transfer/dir&quot;, dir);
    emit transferDirChanged(dir);
}

QString ConfigManager::getHashAlgorithm() const {
    return m_qsettings-&gt;value(&quot;hash/algorithm&quot;, &quot;SHA256&quot;).toString();
}

void ConfigManager::setHashAlgorithm(const QString &amp;algo) {
    m_qsettings-&gt;setValue(&quot;hash/algorithm&quot;, algo);
    emit hashAlgorithmChanged(algo);
}

bool ConfigManager::getAutoTransfer() const {
    return m_qsettings-&gt;value(&quot;transfer/auto&quot;, false).toBool();
}

void ConfigManager::setAutoTransfer(bool enabled) {
    m_qsettings-&gt;setValue(&quot;transfer/auto&quot;, enabled);
    emit autoTransferChanged(enabled);
}

void ConfigManager::loadDefaults() {
    m_qsettings-&gt;setValue(&quot;transfer/dir&quot;, QDir::homePath());
    m_qsettings-&gt;setValue(&quot;hash/algorithm&quot;, &quot;SHA256&quot;);
    m_qsettings-&gt;setValue(&quot;transfer/auto&quot;, false);
}

void ConfigManager::migrateFromOldFormat() {
    // Simple migration example
    if (m_qsettings-&gt;contains(&quot;oldTransferPath&quot;)) {
        setTransferDir(m_qsettings-&gt;value(&quot;oldTransferPath&quot;).toString());
        m_qsettings-&gt;remove(&quot;oldTransferPath&quot;);
    }
}

bool ConfigManager::loadFromJson(const QString &amp;jsonPath) {
    QFile file(jsonPath);
    if (!file.exists()) {
        return false;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        recoverFromCorruptJson();
        return false;
    }
    
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        recoverFromCorruptJson();
        return false;
    }
    
    jsonToSettings(doc.object());
    return true;
}

bool ConfigManager::saveToJson(const QString &amp;jsonPath) {
    QJsonObject json = settingsToJson();
    QJsonDocument doc(json);
    QFile file(jsonPath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    file.write(doc.toJson());
    return true;
}

void ConfigManager::recoverFromCorruptJson() {
    qDebug() &lt;&lt; &quot;Recovering from corrupt JSON, loading defaults&quot;;
    loadDefaults();
}

QJsonObject ConfigManager::settingsToJson() {
    QJsonObject json;
    json[&quot;transfer/dir&quot;] = getTransferDir();
    json[&quot;hash/algorithm&quot;] = getHashAlgorithm();
    json[&quot;transfer/auto&quot;] = getAutoTransfer();
    return json;
}

void ConfigManager::jsonToSettings(const QJsonObject &amp;json) {
    if (json.contains(&quot;transfer/dir&quot;)) setTransferDir(json[&quot;transfer/dir&quot;].toString());
    if (json.contains(&quot;hash/algorithm&quot;)) setHashAlgorithm(json[&quot;hash/algorithm&quot;].toString());
    if (json.contains(&quot;transfer/auto&quot;)) setAutoTransfer(json[&quot;transfer/auto&quot;].toBool());
}

void ConfigManager::onJsonFileChanged(const QString &amp;path) {
    loadAllSettings();
}
