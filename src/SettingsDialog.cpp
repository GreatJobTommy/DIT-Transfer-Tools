#include "SettingsDialog.h"
#include "SettingsManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QDialogButtonBox>

SettingsDialog::SettingsDialog(SettingsManager *settingsManager, QWidget *parent)
    : QDialog(parent)
    , m_settingsManager(settingsManager)
{
    setWindowTitle("Settings");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    tabWidget = new QTabWidget(this);
    mainLayout->addWidget(tabWidget);

    // General Tab
    QWidget *generalTab = new QWidget();
    QVBoxLayout *generalLayout = new QVBoxLayout(generalTab);

    parallelLabel = new QLabel("Parallel Tasks:");
    parallelTasksSpinBox = new QSpinBox();
    parallelTasksSpinBox->setRange(1, 32);
<<<<<<< HEAD
    parallelTasksSpinBox->setValue(m_settingsManager->parallelTasks());

    generalLayout->addWidget(parallelLabel);
    generalLayout->addWidget(parallelTasksSpinBox);

    hashLabel = new QLabel("Hash Algorithm:");
    hashAlgoComboBox = new QComboBox();
    hashAlgoComboBox->addItems({"SHA256", "SHA512"});
<<<<<<< HEAD
    hashAlgoComboBox->setCurrentText(m_settingsManager->hashAlgo());

    generalLayout->addWidget(hashLabel);
    generalLayout->addWidget(hashAlgoComboBox);

    tabWidget->addTab(generalTab, "General");

    // Drive Tab
    QWidget *driveTab = new QWidget();
    QVBoxLayout *driveLayout = new QVBoxLayout(driveTab);

    pollLabel = new QLabel("Drive Poll Interval (s):");
    pollIntervalSpinBox = new QSpinBox();
    pollIntervalSpinBox->setRange(1, 60);
<<<<<<< HEAD
    pollIntervalSpinBox->setValue(m_settingsManager->drivePollInterval());
    pollIntervalSpinBox->setSuffix(" s");

    driveLayout->addWidget(pollLabel);
    driveLayout->addWidget(pollIntervalSpinBox);

    tabWidget->addTab(driveTab, "Drive");

<<<<<<< HEAD
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    saveButton = new QPushButton("Save");
    resetButton = new QPushButton("Reset to Defaults");
    QPushButton *cancelButton = new QPushButton("Cancel");

    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(saveButton, &amp;QPushButton::clicked, this, &amp;SettingsDialog::saveSettings);
    connect(resetButton, &amp;QPushButton::clicked, this, &amp;SettingsDialog::resetSettings);
    connect(cancelButton, &amp;QPushButton::clicked, this, &amp;QDialog::reject);
}

void SettingsDialog::saveSettings() {
    // Save to ConfigManager
    // Note: parallelTasks and pollInterval would need getters/setters added to ConfigManager
    m_configManager->setHashAlgorithm(hashAlgoComboBox->currentText());
    m_configManager->saveAllSettings();
    accept();
}

void SettingsDialog::resetSettings() {
    parallelTasksSpinBox->setValue(4);
    hashAlgoComboBox->setCurrentText("SHA256");
    pollIntervalSpinBox->setValue(5);
}

void SettingsDialog::accept() {
    saveSettings();
=======
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &amp;QDialogButtonBox::accepted, this, &amp;QDialog::accept);
    connect(buttonBox, &amp;QDialogButtonBox::rejected, this, &amp;QDialog::reject);

    connect(m_settingsManager, &amp;SettingsManager::settingsChanged, this, &amp;QDialog::accept);
}

void SettingsDialog::accept() {
    m_settingsManager->setParallelTasks(parallelTasksSpinBox->value());
    m_settingsManager->setHashAlgo(hashAlgoComboBox->currentText());
    m_settingsManager->setDrivePollInterval(pollIntervalSpinBox->value());
    QDialog::accept();
}