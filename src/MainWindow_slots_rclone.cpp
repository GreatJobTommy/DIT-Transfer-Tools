#include "MainWindow.h"
#include "RcloneRemoteDialog.h"
#include <QProcess>
#include <QMessageBox>
#include <QRegularExpression>
#include <QListWidget>

void MainWindow::refreshRcloneRemotes() {
    QString configPath = m_settingsManager->getRcloneConfigPath();
    QProcess* proc = new QProcess(this);
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus) {
        if (exitCode == 0) {
            QString output = QString::fromUtf8(proc->readAllStandardOutput()).trimmed();
            QStringList remotes = output.split('\n', Qt::SkipEmptyParts);
            m_rcloneRemotesList->clear();
            for (const QString& remote : remotes) {
                m_rcloneRemotesList->addItem(remote.replace(QRegularExpression(":$"), ""));
            }
        } else {
            QMessageBox::warning(this, "Rclone Error", "Failed to list remotes: " + QString::fromUtf8(proc->readAllStandardError()));
        }
        proc->deleteLater();
    });
    proc->start("rclone", QStringList() << "listremotes" << "--config" << configPath);
}

void MainWindow::addRcloneRemote() {
    RcloneRemoteDialog dlg(QString(), this);
    if (dlg.exec() == QDialog::Accepted) {
        refreshRcloneRemotes();
    }
}

void MainWindow::editRcloneRemote() {
    QListWidgetItem* item = m_rcloneRemotesList->currentItem();
    if (!item) return;
    QString remote = item->text();
    RcloneRemoteDialog dlg(remote, this);
    if (dlg.exec() == QDialog::Accepted) {
        refreshRcloneRemotes();
    }
}

void MainWindow::deleteRcloneRemote() {
    QListWidgetItem* item = m_rcloneRemotesList->currentItem();
    if (!item) return;
    QString remote = item->text();
    int ret = QMessageBox::question(this, "Confirm Delete", "Delete remote '" + remote + "'?");
    if (ret == QMessageBox::Yes) {
        QString configPath = m_settingsManager->getRcloneConfigPath();
        QProcess proc;
        proc.start("rclone", QStringList() << "config" << "delete" << remote << "--config" << configPath);
        proc.waitForFinished(5000);
        if (proc.exitCode() == 0) {
            refreshRcloneRemotes();
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete remote.");
        }
    }
}