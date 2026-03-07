#include "RcloneRemoteDialog.h"

RcloneRemoteDialog::RcloneRemoteDialog(const QString& existingRemote, QWidget* parent)
 : QDialog(parent), m_existingRemote(existingRemote) {
    setWindowTitle(existingRemote.isEmpty() ? "Add Rclone Remote" : "Edit Rclone Remote");
    setModal(true);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Load presets
    m_presets.clear();
    QFile presetsFile("../presets.json");
    if (presetsFile.exists() && presetsFile.open(QIODevice::ReadOnly)) {
      QJsonDocument doc = QJsonDocument::fromJson(presetsFile.readAll());
      if (doc.isObject()) {
        QJsonObject presetsObj = doc.object();
        for (auto it = presetsObj.constBegin(); it != presetsObj.constEnd(); ++it) {
          m_presets[it.key()] = it.value().toObject();
        }
      }
      presetsFile.close();
    }

    QFormLayout* formLayout = new QFormLayout;

    // Preset combo
    m_presetCombo = new QComboBox();
    m_presetCombo->addItem("Manual SFTP");
    for (auto it = m_presets.constBegin(); it != m_presets.constEnd(); ++it) {
      m_presetCombo->addItem(it.key());
    }
    m_presetCombo->setCurrentIndex(0);
    connect(m_presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RcloneRemoteDialog::loadPreset);
    formLayout->addRow("Preset:", m_presetCombo);

    // Common fields
    m_nameEdit = new QLineEdit(existingRemote);
    formLayout->addRow("Name:", m_nameEdit);

    // SFTP fields
    m_hostEdit = new QLineEdit;
    formLayout->addRow("Host:", m_hostEdit);

    m_userEdit = new QLineEdit("username");
    formLayout->addRow("Username:", m_userEdit);

    m_portSpinBox = new QSpinBox;
    m_portSpinBox->setRange(1, 65535);
    m_portSpinBox->setValue(22);
    formLayout->addRow("Port:", m_portSpinBox);

    m_passEdit = new QLineEdit;
    m_passEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Password:", m_passEdit);

    // Cloud fields
    m_keyIdEdit = new QLineEdit;
    m_keyIdEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Access Key ID:", m_keyIdEdit);

    m_keySecretEdit = new QLineEdit;
    m_keySecretEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Secret Access Key:", m_keySecretEdit);

    m_bucketEdit = new QLineEdit;
    formLayout->addRow("Bucket:", m_bucketEdit);

    m_regionEdit = new QLineEdit("us-east-1");
    formLayout->addRow("Region:", m_regionEdit);

    m_helpLabel = new QLabel("");
    m_helpLabel->setWordWrap(true);
    formLayout->addRow("Help:", m_helpLabel);

    mainLayout->addLayout(formLayout);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    m_cancelBtn = new QPushButton("Cancel");
    m_saveBtn = new QPushButton("Save");
    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_saveBtn);
    mainLayout->addLayout(btnLayout);

    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_saveBtn, &QPushButton::clicked, this, &RcloneRemoteDialog::saveRemote);

    // Initial load
    loadPreset(0);
}

QString RcloneRemoteDialog::remoteName() const {
    return m_nameEdit->text().trimmed();
}

void RcloneRemoteDialog::loadPreset(int index) {
    QString presetName = m_presetCombo->itemText(index);
    m_currentPreset = presetName;
    m_remoteType = "sftp";  // default

    // Clear fields
    m_hostEdit->clear();
    m_userEdit->clear();
    m_passEdit->clear();
    m_keyIdEdit->clear();
    m_keySecretEdit->clear();
    m_bucketEdit->clear();
    m_regionEdit->clear();

    if (presetName == "Manual SFTP") {
        m_remoteType = "sftp";
        m_userEdit->setText("username");
        m_portSpinBox->setValue(22);
        m_helpLabel->setText("Manual SFTP configuration. Fill host, user, password. Password will be obscured.");
    } else if (m_presets.contains(presetName)) {
        QJsonObject preset = m_presets[presetName];
        m_remoteType = preset["type"].toString("sftp");
        m_helpLabel->setText(preset["description"].toString(""));

        QJsonObject params = preset["params"].toObject();
        if (m_remoteType == "s3") {
            m_keyIdEdit->setPlaceholderText("AWS Access Key ID");
            m_keySecretEdit->setPlaceholderText("AWS Secret Access Key");
            m_bucketEdit->setPlaceholderText("S3 Bucket Name");
            m_regionEdit->setText(params["region"].toString("us-east-1"));
            m_helpLabel->setText(preset["description"].toString() + "\nEnter AWS credentials and bucket.");
        } else if (m_remoteType == "b2") {
            m_keyIdEdit->setPlaceholderText("B2 Account ID");
            m_keySecretEdit->setPlaceholderText("B2 Application Key");
            m_bucketEdit->setPlaceholderText("B2 Bucket Name");
            m_helpLabel->setText(preset["description"].toString() + "\nEnter B2 credentials and bucket.");
        } else if (m_remoteType == "drive") {
            m_keyIdEdit->setPlaceholderText("Not needed for Drive (OAuth)");
            m_keySecretEdit->setPlaceholderText("Run 'rclone config' after for OAuth");
            m_bucketEdit->setPlaceholderText("root_folder_id (optional)");
            m_helpLabel->setText(preset["description"].toString() + "\nAfter create, run rclone config to authorize.");
        }
    }
}

void RcloneRemoteDialog::saveRemote() {
    QString name = remoteName();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Remote name is required.");
        return;
    }
    QString host = m_hostEdit->text().trimmed();
    if (host.isEmpty()) {
        QMessageBox::warning(this, "Error", "Host is required.");
        return;
    }
    QString user = m_userEdit->text().trimmed();
    QString pass = m_passEdit->text();
    if (pass.isEmpty()) {
        QMessageBox::warning(this, "Error", "Password is required.");
        return;
    }

    // Obscure password
    QProcess obscure;
    obscure.start("rclone", QStringList() << "obscure" << pass);
    if (!obscure.waitForStarted(3000) || !obscure.waitForFinished(5000)) {
        QMessageBox::warning(this, "Error", "Failed to obscure password.");
        return;
    }
    QString obscured = QString::fromUtf8(obscure.readAllStandardOutput()).trimmed();

    // Delete existing remote if editing
    if (!m_existingRemote.isEmpty()) {
        QProcess del;
        del.start("rclone", QStringList() << "config" << "delete" << m_existingRemote);
        del.waitForFinished(5000);
    }

    // Use rclone config create
    QStringList args;
    args << "config" << "create" << name << "sftp" << "host=" + host << "user=" + user << "port=" + QString::number(m_portSpinBox->value()) << "pass=" + obscured;
    QProcess create;
    create.start("rclone", args);
    if (!create.waitForStarted(3000) || !create.waitForFinished(10000)) {
        QMessageBox::warning(this, "Error", "Failed to create remote: " + create.readAllStandardError());
        return;
    }

    accept();
}