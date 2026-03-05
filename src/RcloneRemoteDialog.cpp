#include "RcloneRemoteDialog.h"

RcloneRemoteDialog::RcloneRemoteDialog(const QString& existingRemote, QWidget* parent)
 : QDialog(parent), m_existingRemote(existingRemote) {
    setWindowTitle(existingRemote.isEmpty() ? "Add Rclone SFTP Remote" : "Edit Rclone SFTP Remote");
    setModal(true);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QFormLayout* formLayout = new QFormLayout;
    m_nameEdit = new QLineEdit(existingRemote);
    formLayout->addRow("Name:", m_nameEdit);

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

    mainLayout->addLayout(formLayout);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    m_cancelBtn = new QPushButton("Cancel");
    m_saveBtn = new QPushButton("Save");
    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_saveBtn);
    mainLayout->addLayout(btnLayout);

    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_saveBtn, &QPushButton::clicked, this, &RcloneRemoteDialog::saveRemote);
}

QString RcloneRemoteDialog::remoteName() const {
    return m_nameEdit->text().trimmed();
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