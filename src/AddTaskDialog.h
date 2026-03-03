#ifndef ADDTASKDIALOG_H
#define ADDTASKDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QListWidget>
#include <QTreeWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDir>
#include <QMessageBox>
#include "TransferTask.h"
#include <QSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QGroupBox>

class FileDropList : public QListWidget {
    Q_OBJECT
public:
    FileDropList(QWidget* parent = nullptr) : QListWidget(parent) {
        setAcceptDrops(true);
    }
protected:
    void dragEnterEvent(QDragEnterEvent* event) override {
        if (event->mimeData()->hasUrls()) {
            event->acceptProposedAction();
        }
    }
    void dropEvent(QDropEvent* event) override {
        const QMimeData* mimeData = event->mimeData();
        if (mimeData->hasUrls()) {
            QList<QUrl> urls = mimeData->urls();
            for (const QUrl& url : urls) {
                if (url.isLocalFile()) {
                    QString path = url.toLocalFile();
                    if (QDir(path).exists()) {
                        addItem(path);
                        emit pathsDropped(QStringList(path));
                    }
                }
            }
            event->acceptProposedAction();
        }
    }
signals:
    void pathsDropped(const QStringList& paths);
};

class AddTaskDialog : public QDialog {
    Q_OBJECT

public:
    AddTaskDialog(QWidget* parent = nullptr);
    ~AddTaskDialog();

    TransferTask* getTask() const;

private slots:
    void browseSource();
    void browseDest();
    void validatePaths();
    void addToQueue();
    void updatePreview();
    void onPathsDropped(const QStringList& paths);
    void onDestChanged();
    void refreshRemotes();
    void updateDestFromRemote();
    void parseRemotePath(const QString &url);

private:
    void setupUI();
    void setupConnections();
    bool isValidPath(const QString& path) const;

    QLineEdit* m_sourceEdit;
    QPushButton* m_sourceBrowseBtn;
    QLineEdit* m_destEdit;
    QPushButton* m_destBrowseBtn;
    QComboBox* m_presetCombo;
    FileDropList* m_dragList; // For drag multi-folder
    QTextEdit* m_previewEdit;
    QPushButton* m_addBtn;
    QPushButton* m_cancelBtn;

    QGroupBox* m_rcloneGroup;
    QComboBox* m_remoteCombo;
    QPushButton* m_refreshRemotesBtn;
    QLineEdit* m_remotePathEdit;
    QPushButton* m_browseRemoteBtn;

    QGroupBox* m_ltoGroup;
    QComboBox* m_ltoPresetCombo;
    QSlider* m_bufferSlider;
    QSpinBox* m_bufferSpinBox;
    QCheckBox* m_verifyCheckBox;

    QStringList m_selectedPaths;
};

#endif // ADDTASKDIALOG_H