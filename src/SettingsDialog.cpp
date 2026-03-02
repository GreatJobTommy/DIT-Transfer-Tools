#include "SettingsDialog.h"
#include "SettingsManager.h"
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QStyle>

SettingsDialog::SettingsDialog(SettingsManager* settings, QWidget* parent)
    : QDialog(parent), m_settings(settings) {
    setWindowTitle("Settings");
    setModal(true);
    resize(600, 700);

    setupUI();
    loadSettings();
}

SettingsDialog::~SettingsDialog() {}

void SettingsDialog::setupUI() {
    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_contentWidget = new QWidget;
    m_layout = new QVBoxLayout(m_contentWidget);

    // Search bar
    m_searchEdit = new QLineEdit;
    m_searchEdit->setPlaceholderText("Search settings...");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &SettingsDialog::onSearchTextChanged);
    m_layout->addWidget(m_searchEdit);

    // Queue Section
    m_queueSection = new QGroupBox("Queue Settings");
    m_queueSection->setCheckable(true);
    m_queueSection->setChecked(true);
    connect(m_queueSection, &QGroupBox::toggled, [this]() { toggleSection(m_queueSection); });

    QFormLayout* queueLayout = new QFormLayout(m_queueSection);
    m_queueSlider = new QSlider(Qt::Horizontal);
    m_queueSlider->setRange(1, 20);
    m_queueLabel = new QLabel;
    connect(m_queueSlider, &QSlider::valueChanged, [this](int value) {
        m_queueLabel->setText(QString::number(value));
    });
    queueLayout->addRow("Max Parallel Transfers:", m_queueSlider);
    queueLayout->addRow("", m_queueLabel);
    m_layout->addWidget(m_queueSection);

    // Hash Section
    m_hashSection = new QGroupBox("Hash Settings");
    m_hashSection->setCheckable(true);
    m_hashSection->setChecked(true);
    connect(m_hashSection, &QGroupBox::toggled, [this]() { toggleSection(m_hashSection); });

    QFormLayout* hashLayout = new QFormLayout(m_hashSection);
    m_hashCombo = new QComboBox;
    m_hashCombo->addItems({"sha256", "sha512"});
    hashLayout->addRow("Algorithm:", m_hashCombo);
    m_layout->addWidget(m_hashSection);

    // Transfer Section
    m_transferSection = new QGroupBox("Transfer Settings");
    m_transferSection->setCheckable(true);
    m_transferSection->setChecked(true);
    connect(m_transferSection, &QGroupBox::toggled, [this]() { toggleSection(m_transferSection); });

    QFormLayout* transferLayout = new QFormLayout(m_transferSection);
    m_chunkAutoCheck = new QCheckBox("Auto");
    m_chunkAutoCheck->setChecked(true);
    connect(m_chunkAutoCheck, &QCheckBox::toggled, [this](bool checked) {
        m_chunkSlider->setEnabled(!checked);
    });
    m_chunkSlider = new QSlider(Qt::Horizontal);
    m_chunkSlider->setRange(1, 100); // MB
    m_chunkSlider->setEnabled(false);
    m_chunkLabel = new QLabel("Auto");
    connect(m_chunkSlider, &QSlider::valueChanged, [this](int value) {
        m_chunkLabel->setText(QString::number(value) + " MB");
    });
    transferLayout->addRow("Chunk Size:", m_chunkAutoCheck);
    transferLayout->addRow("", m_chunkSlider);
    transferLayout->addRow("", m_chunkLabel);
    m_layout->addWidget(m_transferSection);

    // Drives Section
    m_drivesSection = new QGroupBox("Drives Settings");
    m_drivesSection->setCheckable(true);
    m_drivesSection->setChecked(true);
    connect(m_drivesSection, &QGroupBox::toggled, [this]() { toggleSection(m_drivesSection); });

    QFormLayout* drivesLayout = new QFormLayout(m_drivesSection);
    m_scanSlider = new QSlider(Qt::Horizontal);
    m_scanSlider->setRange(5, 60);
    m_scanLabel = new QLabel;
    connect(m_scanSlider, &QSlider::valueChanged, [this](int value) {
        m_scanLabel->setText(QString::number(value) + "s");
    });
    drivesLayout->addRow("Scan Interval:", m_scanSlider);
    drivesLayout->addRow("", m_scanLabel);
    m_layout->addWidget(m_drivesSection);

    // UI Section
    m_uiSection = new QGroupBox("UI Settings");
    m_uiSection->setCheckable(true);
    m_uiSection->setChecked(true);
    connect(m_uiSection, &QGroupBox::toggled, [this]() { toggleSection(m_uiSection); });

    QFormLayout* uiLayout = new QFormLayout(m_uiSection);
    m_themeCheck = new QCheckBox("Dark Theme");
    uiLayout->addRow("", m_themeCheck);
    m_layout->addWidget(m_uiSection);

    // Logs Section
    m_logsSection = new QGroupBox("Logs Settings");
    m_logsSection->setCheckable(true);
    m_logsSection->setChecked(true);
    connect(m_logsSection, &QGroupBox::toggled, [this]() { toggleSection(m_logsSection); });

    QFormLayout* logsLayout = new QFormLayout(m_logsSection);
    m_logsCombo = new QComboBox;
    m_logsCombo->addItems({"debug", "info", "warning", "error"});
    logsLayout->addRow("Level:", m_logsCombo);
    m_layout->addWidget(m_logsSection);

    // Presets Section
    m_presetsSection = new QGroupBox("Presets");
    m_presetsSection->setCheckable(true);
    m_presetsSection->setChecked(true);
    connect(m_presetsSection, &QGroupBox::toggled, [this]() { toggleSection(m_presetsSection); });

    QVBoxLayout* presetsLayout = new QVBoxLayout(m_presetsSection);
    m_presetsList = new QListWidget;
    connect(m_presetsList, &QListWidget::itemDoubleClicked, this, &SettingsDialog::onPresetDoubleClicked);
    presetsLayout->addWidget(m_presetsList);

    QHBoxLayout* presetBtnsLayout = new QHBoxLayout;
    m_presetEdit = new QLineEdit;
    m_presetEdit->setPlaceholderText("New preset name");
    m_addPresetBtn = new QPushButton("Add");
    connect(m_addPresetBtn, &QPushButton::clicked, this, &SettingsDialog::onAddPresetClicked);
    m_removePresetBtn = new QPushButton("Remove");
    connect(m_removePresetBtn, &QPushButton::clicked, this, &SettingsDialog::onRemovePresetClicked);
    presetBtnsLayout->addWidget(m_presetEdit);
    presetBtnsLayout->addWidget(m_addPresetBtn);
    presetBtnsLayout->addWidget(m_removePresetBtn);
    presetsLayout->addLayout(presetBtnsLayout);
    m_layout->addWidget(m_presetsSection);

    // Advanced Section
    m_advancedSection = new QGroupBox("Advanced");
    m_advancedSection->setCheckable(true);
    m_advancedSection->setChecked(false); // Collapsed by default
    connect(m_advancedSection, &QGroupBox::toggled, [this]() { toggleSection(m_advancedSection); });

    QFormLayout* advancedLayout = new QFormLayout(m_advancedSection);
    m_rcloneCheck = new QCheckBox("Enable Advanced Rclone Features");
    advancedLayout->addRow("", m_rcloneCheck);
    m_layout->addWidget(m_advancedSection);

    m_layout->addStretch();

    // Bottom buttons
    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    m_saveJsonBtn = new QPushButton("Save to JSON");
    connect(m_saveJsonBtn, &QPushButton::clicked, this, &SettingsDialog::onSaveClicked);
    m_loadJsonBtn = new QPushButton("Load from JSON");
    connect(m_loadJsonBtn, &QPushButton::clicked, this, &SettingsDialog::onLoadClicked);
    m_resetBtn = new QPushButton("Reset to Defaults");
    connect(m_resetBtn, &QPushButton::clicked, this, &SettingsDialog::onResetClicked);
    m_okBtn = new QPushButton("OK");
    connect(m_okBtn, &QPushButton::clicked, this, &QDialog::accept);
    m_cancelBtn = new QPushButton("Cancel");
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    buttonsLayout->addWidget(m_saveJsonBtn);
    buttonsLayout->addWidget(m_loadJsonBtn);
    buttonsLayout->addWidget(m_resetBtn);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(m_okBtn);
    buttonsLayout->addWidget(m_cancelBtn);
    m_layout->addLayout(buttonsLayout);

    m_scrollArea->setWidget(m_contentWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_scrollArea);
}

void SettingsDialog::loadSettings() {
    m_queueSlider->setValue(m_settings->getMaxParallelTransfers());
    m_hashCombo->setCurrentText(m_settings->getHashAlgorithm());
    int chunkSize = m_settings->getTransferChunkSize();
    if (chunkSize == 0) {
        m_chunkAutoCheck->setChecked(true);
        m_chunkSlider->setEnabled(false);
        m_chunkLabel->setText("Auto");
    } else {
        m_chunkAutoCheck->setChecked(false);
        m_chunkSlider->setValue(chunkSize);
        m_chunkSlider->setEnabled(true);
        m_chunkLabel->setText(QString::number(chunkSize) + " MB");
    }
    m_scanSlider->setValue(m_settings->getDrivesScanInterval());
    m_themeCheck->setChecked(m_settings->getUIThemeDark());
    m_logsCombo->setCurrentText(m_settings->getLogsLevel());
    m_presetsList->addItems(m_settings->getPresets());
    m_rcloneCheck->setChecked(m_settings->getAdvancedRclone());
}

void SettingsDialog::saveSettings() {
    m_settings->setMaxParallelTransfers(m_queueSlider->value());
    m_settings->setHashAlgorithm(m_hashCombo->currentText());
    int chunkSize = m_chunkAutoCheck->isChecked() ? 0 : m_chunkSlider->value();
    m_settings->setTransferChunkSize(chunkSize);
    m_settings->setDrivesScanInterval(m_scanSlider->value());
    m_settings->setUIThemeDark(m_themeCheck->isChecked());
    m_settings->setLogsLevel(m_logsCombo->currentText());
    QStringList presets;
    for (int i = 0; i < m_presetsList->count(); ++i) {
        presets.append(m_presetsList->item(i)->text());
    }
    m_settings->setPresets(presets);
    m_settings->setAdvancedRclone(m_rcloneCheck->isChecked());
}

void SettingsDialog::toggleSection(QGroupBox* groupBox) {
    // When collapsed, hide contents but keep title
    // Qt's checkable groupbox handles visibility automatically via layout
}

void SettingsDialog::onSaveClicked() {
    QString filePath = QFileDialog::getSaveFileName(this, "Save Settings", "", "JSON Files (*.json)");
    if (!filePath.isEmpty()) {
        if (m_settings->saveToJson(filePath)) {
            QMessageBox::information(this, "Success", "Settings saved to JSON.");
        } else {
            QMessageBox::critical(this, "Error", "Failed to save settings.");
        }
    }
}

void SettingsDialog::onLoadClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Load Settings", "", "JSON Files (*.json)");
    if (!filePath.isEmpty()) {
        if (m_settings->loadFromJson(filePath)) {
            loadSettings();
            QMessageBox::information(this, "Success", "Settings loaded from JSON.");
        } else {
            QMessageBox::critical(this, "Error", "Failed to load settings.");
        }
    }
}

void SettingsDialog::onResetClicked() {
    if (QMessageBox::question(this, "Reset", "Reset all settings to defaults?") == QMessageBox::Yes) {
        m_settings->resetToDefaults();
        loadSettings();
    }
}

void SettingsDialog::onSearchTextChanged(const QString& text) {
    filterSections(text);
}

void SettingsDialog::filterSections(const QString& text) {
    // Simple filter: show/hide sections based on title match
    QString lowerText = text.toLower();
    m_queueSection->setVisible(lowerText.isEmpty() || "queue".contains(lowerText));
    m_hashSection->setVisible(lowerText.isEmpty() || "hash".contains(lowerText));
    m_transferSection->setVisible(lowerText.isEmpty() || "transfer".contains(lowerText));
    m_drivesSection->setVisible(lowerText.isEmpty() || "drives".contains(lowerText));
    m_uiSection->setVisible(lowerText.isEmpty() || "ui".contains(lowerText));
    m_logsSection->setVisible(lowerText.isEmpty() || "logs".contains(lowerText));
    m_presetsSection->setVisible(lowerText.isEmpty() || "presets".contains(lowerText));
    m_advancedSection->setVisible(lowerText.isEmpty() || "advanced".contains(lowerText));
}

void SettingsDialog::onAddPresetClicked() {
    QString preset = m_presetEdit->text().trimmed();
    if (!preset.isEmpty() && m_presetsList->findItems(preset, Qt::MatchExactly).isEmpty()) {
        m_presetsList->addItem(preset);
        m_presetEdit->clear();
    }
}

void SettingsDialog::onRemovePresetClicked() {
    QListWidgetItem* item = m_presetsList->currentItem();
    if (item) {
        delete item;
    }
}

void SettingsDialog::onPresetDoubleClicked(QListWidgetItem* item) {
    m_presetEdit->setText(item->text());
    onRemovePresetClicked();
}