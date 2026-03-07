#ifndef ADDTASKDIALOG_H
#define ADDTASKDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QTextEdit>
#include <QSplitter>
#include "DragDropList.h"
#include "TransferTask.h"

class AddTaskDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddTaskDialog(QWidget *parent = nullptr);
    ~AddTaskDialog();

    QList<TransferTask*> getTasks() const;

private slots:
    void browseSource();
    void browseDest();
    void validatePaths();
    void addToQueue();
    void onPathsDropped(const QStringList& paths);
    void onDestChanged();
    void parseRemotePath(const QString &url);
    void updateDestFromRemote();
    void refreshRemotes();
    void onPresetChanged(int index);
    void onLtfsDriveChanged(int index);
    void updatePreview();

private:
    void setupUI();
    void setupConnections();
    bool isValidPath(const QString& path) const;

    QLineEdit *m_sourceEdit;
    QPushButton *m_sourceBrowseBtn;
    QLineEdit *m_destEdit;
    QPushButton *m_destBrowseBtn;
    QComboBox *m_presetCombo;
    QGroupBox *m_rcloneGroup;
    QComboBox *m_remoteCombo;
    QPushButton *m_refreshRemotesBtn;
    QLineEdit *m_remotePathEdit;
    QPushButton *m_browseRemoteBtn;
    DragDropList *m_dragList;
    QTextEdit *m_previewEdit;
    QPushButton *m_addBtn;
    QPushButton *m_cancelBtn;
    QCheckBox *m_verifyCheckBox;
    QGroupBox *m_ltfsGroup;
    QLabel *m_ltfsLabel;
    QComboBox *m_ltfsCombo;
    QStringList m_selectedPaths;
};

#endif // ADDTASKDIALOG_H
