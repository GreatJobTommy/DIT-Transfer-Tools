#ifndef ADDTASKDIALOG_H
#define ADDTASKDIALOG_H

#include &lt;QDialog&gt;
#include &lt;QDragEnterEvent&gt;
#include &lt;QDropEvent&gt;
#include &lt;QMimeData&gt;
#include &lt;QDirIterator&gt;
#include &lt;QFileInfo&gt;
#include &lt;QJsonObject&gt;
#include &quot;TransferTask.h&quot;

namespace Ui {
class AddTaskDialog;
}

class AddTaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTaskDialog(QWidget *parent = nullptr);
    ~AddTaskDialog();
    QStringList getSources() const;
    QStringList getTargets() const;
    QJsonObject getPresetSettings() const;

private slots:
    void onPresetChanged(int index);
    void onPreviewClicked();
    void onDropZoneDrop(QDropEvent *event);
    void onDropZoneDragEnter(QDragEnterEvent *event);

private:
    Ui::AddTaskDialog *ui;
    QStringList m_sources;
    QJsonObject m_presetSettings;
    void updatePreview();
    bool validateInputs();
    qint64 calculateTotalSize(const QStringList &amp;paths);
    QString estimateTime(qint64 bytes);
    void loadSmartPresets();
};

#endif // ADDTASKDIALOG_H
