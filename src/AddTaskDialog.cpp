#include "AddTaskDialog.h"
#include <QGroupBox>
#include <QFormLayout>
#include <QSplitter>
#include <QApplication>
#include <QUrl>

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

    // Destinations list
    QLabel* destLabel = new QLabel("Destinations:");
    m_destList = new QListWidget;
    m_destList->setMaximumHeight(100);
    m_addDestBtn = new QPushButton("Add Dest...");
    m_removeDestBtn = new QPushButton("Remove");
    QHBoxLayout* destBtnLayout = new QHBoxLayout;
    destBtnLayout->addWidget(m_addDestBtn);
    destBtnLayout->addWidget(m_removeDestBtn);
    pathLayout->addRow(destLabel);
    pathLayout->addRow(m_destList);
    pathLayout->addRow(destBtnLayout);

    mainLayout->addWidget(pathGroup);

    // Presets
    QGroupBox* presetGroup = new QGroupBox("Presets");
    QHBoxLayout* presetLayout = new QHBoxLayout(presetGroup);
    m_presetCombo = new QComboBox;
    m_presetCombo->addItem("Default");
    m_presetCombo->addItem("Fast Copy");
    m_presetCombo->addItem("Safe Copy");
    presetLayout->addWidget(new QLabel("Preset:"));
    presetLayout->addWidget(m_presetCombo);
    presetLayout->addStretch();
    mainLayout->addWidget(presetGroup);

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
    connect(m_addDestBtn, &QPushButton::clicked, this, &AddTaskDialog::addDestination);
    connect(m_removeDestBtn, &QPushButton::clicked, this, &AddTaskDialog::removeDestination);
    connect(m_sourceEdit, &QLineEdit::textChanged, this, &AddTaskDialog::validatePaths);
    connect(m_addBtn, &QPushButton::clicked, this, &AddTaskDialog::addToQueue);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_dragList, &FileDropList::pathsDropped, this, &AddTaskDialog::onPathsDropped);
}

void AddTaskDialog::browseSource() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Source Directory", m_sourceEdit->text());
    if (!dir.isEmpty()) {
        m_sourceEdit->setText(dir);
    }
}

void AddTaskDialog::addDestination() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Destination Directory");
    if (!dir.isEmpty()) {
        m_destList->addItem(dir);
    }
}

void AddTaskDialog::removeDestination() {
    QList<QListWidgetItem*> selected = m_destList->selectedItems();
    for (QListWidgetItem* item : selected) {
        delete item;
    }
}

void AddTaskDialog::validatePaths() {
    QString style = "QLineEdit { border: 1px solid %1; }";
    if (isValidPath(m_sourceEdit->text())) {
        m_sourceEdit->setStyleSheet(style.arg("green"));
    } else {
        m_sourceEdit->setStyleSheet(style.arg("red"));
    }
}

bool AddTaskDialog::isValidPath(const QString& path) const {
    return !path.isEmpty() && QDir(path).exists();
}

void AddTaskDialog::addToQueue() {
    if (!isValidPath(m_sourceEdit->text()) || m_destList->count() == 0) {
        QMessageBox::warning(this, "Invalid Paths", "Please select valid source and at least one destination directory.");
        return;
    }
    accept();
}

TransferTask* AddTaskDialog::getTask() const {
    QMap<QString, QString> dests;
    for (int i = 0; i < m_destList->count(); ++i) {
        QString dest = m_destList->item(i)->text();
        dests[QString("dest%1").arg(i)] = dest; // Simple key
    }
    return new TransferTask(m_sourceEdit->text(), dests);
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