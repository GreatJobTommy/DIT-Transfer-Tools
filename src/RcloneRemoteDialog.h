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

class RcloneRemoteDialog : public QDialog {
    Q_OBJECT

public:
    RcloneRemoteDialog(const QString& existingRemote = QString(), QWidget* parent = nullptr);
    QString remoteName() const;

private slots:
    void saveRemote();

private:
    QLineEdit* m_nameEdit;
    QLineEdit* m_hostEdit;
    QLineEdit* m_userEdit;
    QSpinBox* m_portSpinBox;
    QLineEdit* m_passEdit;
    QPushButton* m_saveBtn;
    QPushButton* m_cancelBtn;
    QString m_existingRemote;
};

#endif