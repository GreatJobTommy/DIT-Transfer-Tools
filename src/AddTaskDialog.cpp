#include &quot;AddTaskDialog.h&quot;
#include &quot;ui_AddTaskDialog.h&quot;
#include &lt;QDebug&gt;
#include &lt;QFileInfo&gt;
#include &lt;QDir&gt;
#include &lt;QJsonDocument&gt;
#include &lt;QMessageBox&gt;
#include &lt;QDirIterator&gt;
#include &lt;QCryptographicHash&gt;

AddTaskDialog::AddTaskDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AddTaskDialog)
{
    ui-&gt;setupUi(this);
    setAcceptDrops(true);
    
    ui-&gt;dropZone-&gt;setAcceptDrops(true);
    ui-&gt;previewEdit-&gt;setPlainText(&quot;Drop files/folders to preview...&quot;);
    
    loadSmartPresets();
    connect(ui-&gt;presetCombo, QOverload&lt;int&gt;::of(&amp;QComboBox::currentIndexChanged), this, &amp;AddTaskDialog::onPresetChanged);
    connect(ui-&gt;previewButton, &amp;QPushButton::clicked, this, &amp;AddTaskDialog::onPreviewClicked);
}

AddTaskDialog::~AddTaskDialog()
{
    delete ui;
}

QStringList AddTaskDialog::getSources() const { return m_sources; }

QStringList AddTaskDialog::getTargets() const {
    return ui-&gt;targetEdit-&gt;text().split(&#39;,&#39;, Qt::SkipEmptyParts);
}

QJsonObject AddTaskDialog::getPresetSettings() const { return m_presetSettings; }

void AddTaskDialog::loadSmartPresets() {
    ui-&gt;presetCombo-&gt;clear();
    ui-&gt;presetCombo-&gt;addItems({&quot;Fast Copy (no verify)&quot;, &quot;Safe (chunk 1MB + hash)&quot;, &quot;USB Backup (verify + resume)&quot;, &quot;Custom&quot;});
    onPresetChanged(0);
}

void AddTaskDialog::onPresetChanged(int index) {
    switch(index) {
    case 0: // Fast
        m_presetSettings = { {&quot;chunkSize&quot;, 64*1024*1024}, {&quot;verify&quot;, false}, {&quot;hash&quot;, false} };
        break;
    case 1: // Safe
        m_presetSettings = { {&quot;chunkSize&quot;, 1*1024*1024}, {&quot;verify&quot;, true}, {&quot;hash&quot;, true} };
        break;
    case 2: // USB
        m_presetSettings = { {&quot;chunkSize&quot;, 4*1024*1024}, {&quot;verify&quot;, true}, {&quot;hash&quot;, true}, {&quot;resume&quot;, true} };
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
    
    preview += QString(&quot;Sources (%1):\n&quot;).arg(m_sources.size());
    for (const QString&amp; path : m_sources) {
        QFileInfo fi(path);
        preview += QString(&quot;  %1 (%2)\n&quot;).arg(fi.fileName(), fi.isDir() ? &quot;DIR&quot; : humanReadableSize(fi.size()));
    }
    preview += QString(&quot;\nTotal: %1 | Est. Time: %2\n&quot;).arg(humanReadableSize(totalSize), estTime);
    preview += QString(&quot;Preset: %1\n&quot;).arg(ui-&gt;presetCombo-&gt;currentText());
    preview += m_presetSettings.isEmpty() ? &quot;&quot; : QJsonDocument(m_presetSettings).toJson(QJsonDocument::Compact);
    
    if (validateInputs()) {
        preview += &quot;\n✅ VALIDATED&quot;;
    } else {
        preview += &quot;\n❌ INVALID: Check targets/paths&quot;;
    }
    
    ui-&gt;previewEdit-&gt;setPlainText(preview);
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
                if (total &gt; 100LL*1024*1024*1024*1024) break; // Cap at 100TB
            }
        }
    }
    return total;
}

QString AddTaskDialog::estimateTime(qint64 bytes) {
    double speed = 100.0; // MB/s average
    double secs = (bytes / 1024.0 / 1024.0) / speed;
    int mins = secs / 60;
    return QString(&quot;%1min&quot;).arg(mins);
}

QString AddTaskDialog::humanReadableSize(qint64 bytes) {
    QStringList units = {&quot;B&quot;, &quot;KB&quot;, &quot;MB&quot;, &quot;GB&quot;, &quot;TB&quot;};
    double size = bytes;
    int unit = 0;
    while (size &gt;= 1024 &amp;&amp; unit &lt; units.size()-1) {
        size /= 1024;
        ++unit;
    }
    return QString(&quot;%1 %2&quot;).arg(size, 0, &#39;f&#39;, 1).arg(units[unit]);
}

void AddTaskDialog::onDropZoneDragEnter(QDragEnterEvent *event) {
    if (event-&gt;mimeData()-&gt;hasUrls()) {
        event-&gt;acceptProposedAction();
    }
}

void AddTaskDialog::onDropZoneDrop(QDropEvent *event) {
    m_sources.clear();
    QList&lt;QUrl&gt; urls = event-&gt;mimeData()-&gt;urls();
    for (const QUrl&amp; url : urls) {
        QString path = url.toLocalFile();
        if (QFileInfo::exists(path)) {
            m_sources &lt;&lt; path;
        }
    }
    updatePreview();
    event-&gt;acceptProposedAction();
}
