#ifndef RCLONEREMOTEDIALOG_H
#define RCLONEREMOTEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include <QComboBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>

class RcloneRemoteDialog : public QDialog {
    Q_OBJECT

public:
    RcloneRemoteDialog(const QString& existingRemote = QString(), QWidget* parent = nullptr);
    QString remoteName() const;

private slots:
    void saveRemote();
    void loadPreset(int index);

private:
    QLineEdit* m_nameEdit;
    QLineEdit* m_hostEdit;
    QLineEdit* m_userEdit;
    QSpinBox* m_portSpinBox;
    QLineEdit* m_passEdit;
    QLineEdit* m_keyIdEdit;
    QLineEdit* m_keySecretEdit;
    QLineEdit* m_bucketEdit;
    QLineEdit* m_regionEdit;
    QComboBox* m_presetCombo;
    QPushButton* m_saveBtn;
    QPushButton* m_cancelBtn;
    QMap&lt;QString, QJsonObject&gt; m_presets;
    QString m_existingRemote;
    QString m_remoteType;
    QString m_currentPreset;
    QLabel* m_helpLabel;
};

#endif