#include "AddTaskDialog.h"
#include <QGroupBox>
#include <QFormLayout>
#include <QSplitter>
#include <QApplication>
#include <QUrl>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QMessageBox>
#include <QList>

AddTaskDialog::AddTaskDialog(QWidget* parent)
    : QDialog(parent), m_selectedPaths(QStringList()) {
    setupUI();
    setupConnections();
    setWindowTitle("Add Transfer Task");
    setModal(true);
    resize(600, 400);
}

AddTaskDialog::~AddTaskDialog() {}

void AddTaskDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Source/Dest group
    QGroupBox* pathGroup = new QGroupBox("Paths");
    QFormLayout* pathLayout = new QFormLayout(pathGroup);

    m_sourceEdit = new QLineEdit;
    m_sourceBrowseBtn = new QPushButton("Browse...");
    QHBoxLayout* sourceLayout = new QHBoxLayout;
    sourceLayout->addWidget(m_sourceEdit);
    sourceLayout->addWidget(m_sourceBrowseBtn);
    pathLayout->addRow("Source:", sourceLayout);

    m_destEdit = new QLineEdit;
    m_destBrowseBtn = new QPushButton("Browse...");
    QHBoxLayout* destLayout = new QHBoxLayout;
    destLayout->addWidget(m_destEdit);
    destLayout->addWidget(m_destBrowseBtn);
    pathLayout->addRow("Destination:", destLayout);

    mainLayout->addWidget(pathGroup);

    // Presets
    QGroupBox* presetGroup = new QGroupBox("Presets");
    QHBoxLayout* presetLayout = new QHBoxLayout(presetGroup);
    m_presetCombo = new QComboBox;
    m_presetCombo->addItem("Default");
    m_presetCombo->addItem("Fast Copy");
    m_presetCombo->addItem("Safe Copy");
    m_presetCombo->addItem("S3 Fast");
    m_presetCombo->addItem("GDrive");
    m_presetCombo->addItem("LTO Tape");
    presetLayout->addWidget(new QLabel("Preset:"));
    presetLayout->addWidget(m_presetCombo);
    presetLayout->addStretch();
    mainLayout->addWidget(presetGroup);

    // Rclone
    m_rcloneGroup = new QGroupBox("Rclone Remote");
    QFormLayout* rcloneLayout = new QFormLayout(m_rcloneGroup);
    QHBoxLayout* remoteLayout = new QHBoxLayout();
    m_remoteCombo = new QComboBox();
    remoteLayout->addWidget(m_remoteCombo);
    m_refreshRemotesBtn = new QPushButton("Refresh");
    remoteLayout->addWidget(m_refreshRemotesBtn);
    rcloneLayout->addRow("Remote:", remoteLayout);

    QHBoxLayout* remotePathLayout = new QHBoxLayout();
    m_remotePathEdit = new QLineEdit("/");
    remotePathLayout->addWidget(m_remotePathEdit);
    m_browseRemoteBtn = new QPushButton("Browse Path");
    remotePathLayout->addWidget(m_browseRemoteBtn);
    rcloneLayout->addRow("Path:", remotePathLayout);
    mainLayout->addWidget(m_rcloneGroup);
    m_rcloneGroup->hide();

    // Drag Multi-Folder and Preview
    QSplitter* splitter = new QSplitter(Qt::Horizontal);

    QGroupBox* dragGroup = new QGroupBox("Drag Folders Here");
    QVBoxLayout* dragLayout = new QVBoxLayout(dragGroup);
    m_dragList = new FileDropList;
    dragLayout->addWidget(m_dragList);
    splitter->addWidget(dragGroup);

    QGroupBox* previewGroup = new QGroupBox("Preview");
    QVBoxLayout* previewLayout = new QVBoxLayout(previewGroup);
    m_previewEdit = new QTextEdit;
    m_previewEdit->setReadOnly(true);
    previewLayout->addWidget(m_previewEdit);
    splitter->addWidget(previewGroup);

    mainLayout->addWidget(splitter);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    m_addBtn = new QPushButton("Add to Queue");
    m_cancelBtn = new QPushButton("Cancel");
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelBtn);
    buttonLayout->addWidget(m_addBtn);
    mainLayout->addLayout(buttonLayout);
}

void AddTaskDialog::setupConnections() {
    connect(m_sourceBrowseBtn, &QPushButton::clicked, this, &AddTaskDialog::browseSource);
    connect(m_destBrowseBtn, &QPushButton::clicked, this, &AddTaskDialog::browseDest);
    connect(m_sourceEdit, &QLineEdit::textChanged, this, &AddTaskDialog::validatePaths);
    connect(m_destEdit, &QLineEdit::textChanged, this, &AddTaskDialog::validatePaths);
    connect(m_addBtn, &QPushButton::clicked, this, &AddTaskDialog::addToQueue);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_dragList, &FileDropList::pathsDropped, this, &AddTaskDialog::onPathsDropped);
    connect(m_destEdit, &QLineEdit::textChanged, this, &AddTaskDialog::onDestChanged);
    connect(m_refreshRemotesBtn, &QPushButton::clicked, this, &AddTaskDialog::refreshRemotes);
    connect(m_remoteCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AddTaskDialog::updateDestFromRemote);
    connect(m_remotePathEdit, &QLineEdit::textChanged, this, &AddTaskDialog::updateDestFromRemote);
}

void AddTaskDialog::browseSource() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Source Directory", m_sourceEdit->text());
    if (!dir.isEmpty()) {
        m_sourceEdit->setText(dir);
    }
}

void AddTaskDialog::browseDest() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Destination Directory", m_destEdit->text());
    if (!dir.isEmpty()) {
        m_destEdit->setText(dir);
    }
}

void AddTaskDialog::validatePaths() {
    QString style = "QLineEdit { border: 1px solid %1; }";
    if (isValidPath(m_sourceEdit->text())) {
        m_sourceEdit->setStyleSheet(style.arg("green"));
    } else {
        m_sourceEdit->setStyleSheet(style.arg("red"));
    }
    if (isValidPath(m_destEdit->text())) {
        m_destEdit->setStyleSheet(style.arg("green"));
    } else {
        m_destEdit->setStyleSheet(style.arg("red"));
    }
}

bool AddTaskDialog::isValidPath(const QString& path) const {
    return !path.isEmpty() && QDir(path).exists();
}

void AddTaskDialog::addToQueue() {
    if (!isValidPath(m_sourceEdit->text()) || !isValidPath(m_destEdit->text())) {
        QMessageBox::warning(this, "Invalid Paths", "Please select valid source and destination directories.");
        return;
    }
    accept();
}

QList<TransferTask*> AddTaskDialog::getTasks() const {
    QList<TransferTask*> tasks;
    for (const QString& path : m_selectedPaths) {
        TransferTask* task = new TransferTask(path, m_destEdit->text());
        task->setPreset(m_presetCombo->currentText());
        task->setVerifyEnabled(m_verifyCheckBox ? m_verifyCheckBox->isChecked() : false);
        tasks.append(task);
    }
    if (tasks.isEmpty()) {
        TransferTask* task = new TransferTask(m_sourceEdit->text(), m_destEdit->text());
        task->setPreset(m_presetCombo->currentText());
        task->setVerifyEnabled(m_verifyCheckBox ? m_verifyCheckBox->isChecked() : false);
        tasks.append(task);
    }
    return tasks;
}

void AddTaskDialog::updatePreview() {
    QStringList files;
    for (const QString& path : m_selectedPaths) {
        QDir dir(path);
        QStringList entries = dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
        files.append(path + ": " + entries.join(", "));
    }
    m_previewEdit->setPlainText(files.join("\n"));
}

void AddTaskDialog::onPathsDropped(const QStringList& paths) {
    m_selectedPaths.append(paths);
    updatePreview();
}

void AddTaskDialog::onDestChanged() {
    QString dest = m_destEdit->text().trimmed();
    if (dest.startsWith("rclone://")) {
        parseRemotePath(dest);
        m_rcloneGroup->show();
    } else {
        m_rcloneGroup->hide();
    }
    validatePaths();
}

void AddTaskDialog::parseRemotePath(const QString &url) {
    if (!url.startsWith("rclone://")) return;
    QString raw = url.mid(9);
    int colPos = raw.indexOf(':');
    if (colPos == -1) return;
    QString remote = raw.left(colPos + 1);
    QString path = raw.mid(colPos + 1);
    int idx = m_remoteCombo->findText(remote);
    if (idx != -1) m_remoteCombo->setCurrentIndex(idx);
    m_remotePathEdit->setText(path);
}

void AddTaskDialog::updateDestFromRemote() {
    if (m_remoteCombo->currentIndex() == -1) return;
    QString remote = m_remoteCombo->currentText();
    QString path = m_remotePathEdit->text();
    if (!path.isEmpty() && !path.startsWith("/")) path.prepend("/");
    m_destEdit->setText("rclone://" + remote + path);
}

void AddTaskDialog::refreshRemotes() {
    QProcess *proc = new QProcess(this);
    QSettings settings;
    QString configPath = settings.value("rclone/configPath", QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/rclone/rclone.conf").toString();
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus status) {
        if (exitCode == 0 && status == QProcess::NormalExit) {
            QString output = QString::fromUtf8(proc->readAllStandardOutput()).trimmed();
            QStringList remotes = output.split("\\n", Qt::SkipEmptyParts);
            m_remoteCombo->clear();
            for (const QString &r : remotes) {
                m_remoteCombo->addItem(r.trimmed());
            }
        } else {
            QMessageBox::warning(this, "Rclone Error", "Could not list remotes. Ensure rclone config exists.");
        }
        proc->deleteLater();
    });
    proc->start("rclone", QStringList() << "listremotes" << "--config" << configPath);
}