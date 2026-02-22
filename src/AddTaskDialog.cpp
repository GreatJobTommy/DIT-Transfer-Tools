#include "AddTaskDialog.h"
#include "ui_AddTaskDialog.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QMessageBox>
#include <QDirIterator>
#include <QCryptographicHash>

AddTaskDialog::AddTaskDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AddTaskDialog)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    
    ui->dropZone->setAcceptDrops(true);
    ui->previewEdit->setPlainText("Drop files/folders to preview...");
    
    loadSmartPresets();
    connect(ui->presetCombo, QOverload<int>::of(&amp;QComboBox::currentIndexChanged), this, &amp;AddTaskDialog::onPresetChanged);
    connect(ui->previewButton, &amp;QPushButton::clicked, this, &amp;AddTaskDialog::onPreviewClicked);
}

AddTaskDialog::~AddTaskDialog()
{
    delete ui;
}

QStringList AddTaskDialog::getSources() const { return m_sources; }

QStringList AddTaskDialog::getTargets() const {
    return ui->targetEdit->text().split(',', Qt::SkipEmptyParts);
}

QJsonObject AddTaskDialog::getPresetSettings() const { return m_presetSettings; }

void AddTaskDialog::loadSmartPresets() {
    ui->presetCombo->clear();
    ui->presetCombo->addItems({"Fast Copy (no verify)", "Safe (chunk 1MB + hash)", "USB Backup (verify + resume)", "Custom"});
    onPresetChanged(0);
}

void AddTaskDialog::onPresetChanged(int index) {
    switch(index) {
    case 0: // Fast
        m_presetSettings = { {"chunkSize", 64*1024*1024}, {"verify", false}, {"hash", false} };
        break;
    case 1: // Safe
        m_presetSettings = { {"chunkSize", 1*1024*1024}, {"verify", true}, {"hash", true} };
        break;
    case 2: // USB
        m_presetSettings = { {"chunkSize", 4*1024*1024}, {"verify", true}, {"hash", true}, {"resume", true} };
        break;
    default:
        m_presetSettings = {};
    }
    if (!m_sources.isEmpty()) updatePreview();
}

void AddTaskDialog::onPreviewClicked() {
    if (!validateInputs()) return;
    updatePreview();
}

void AddTaskDialog::updatePreview() {
    QString preview;
    qint64 totalSize = calculateTotalSize(m_sources);
    QString estTime = estimateTime(totalSize);
    
    preview += QString("Sources (%1):\n").arg(m_sources.size());
    for (const QString&amp; path : m_sources) {
        QFileInfo fi(path);
        preview += QString("  %1 (%2)\n").arg(fi.fileName(), fi.isDir() ? "DIR" : humanReadableSize(fi.size()));
    }
    preview += QString("\nTotal: %1 | Est. Time: %2\n").arg(humanReadableSize(totalSize), estTime);
    preview += QString("Preset: %1\n").arg(ui->presetCombo->currentText());
    preview += m_presetSettings.isEmpty() ? "" : QJsonDocument(m_presetSettings).toJson(QJsonDocument::Compact);
    
    if (validateInputs()) {
        preview += "\n✅ VALIDATED";
    } else {
        preview += "\n❌ INVALID: Check targets/paths";
    }
    
    ui->previewEdit->setPlainText(preview);
}

bool AddTaskDialog::validateInputs() {
    if (m_sources.isEmpty()) return false;
    QStringList targets = getTargets();
    if (targets.isEmpty()) return false;
    for (const QString&amp; t : targets) {
        if (!QDir(t).exists() &amp;&amp; !QFile(t).exists()) return false;
    }
    return true;
}

qint64 AddTaskDialog::calculateTotalSize(const QStringList &amp;paths) {
    qint64 total = 0;
    for (const QString&amp; path : paths) {
        if (QFileInfo(path).isFile()) {
            total += QFileInfo(path).size();
        } else if (QFileInfo(path).isDir()) {
            QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                total += QFileInfo(it.next()).size();
                if (total > 100LL*1024*1024*1024*1024) break; // Cap at 100TB
            }
        }
    }
    return total;
}

QString AddTaskDialog::estimateTime(qint64 bytes) {
    double speed = 100.0; // MB/s average
    double secs = (bytes / 1024.0 / 1024.0) / speed;
    int mins = secs / 60;
    return QString("%1min").arg(mins);
}

QString AddTaskDialog::humanReadableSize(qint64 bytes) {
    QStringList units = {"B", "KB", "MB", "GB", "TB"};
    double size = bytes;
    int unit = 0;
    while (size >= 1024 &amp;&amp; unit < units.size()-1) {
        size /= 1024;
        ++unit;
    }
    return QString("%1 %2").arg(size, 0, 'f', 1).arg(units[unit]);
}

void AddTaskDialog::onDropZoneDragEnter(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void AddTaskDialog::onDropZoneDrop(QDropEvent *event) {
    m_sources.clear();
    QList<QUrl> urls = event->mimeData()->urls();
    for (const QUrl&amp; url : urls) {
        QString path = url.toLocalFile();
        if (QFileInfo::exists(path)) {
            m_sources << path;
        }
    }
    updatePreview();
    event->acceptProposedAction();
}
