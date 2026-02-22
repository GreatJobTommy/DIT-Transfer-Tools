#include &quot;SettingsDialog.h&quot;
#include &quot;SettingsManager.h&quot;
#include &lt;QVBoxLayout&gt;
#include &lt;QHBoxLayout&gt;
#include &lt;QTabWidget&gt;
#include &lt;QPushButton&gt;
#include &lt;QSpinBox&gt;
#include &lt;QComboBox&gt;
#include &lt;QDialogButtonBox&gt;

SettingsDialog::SettingsDialog(SettingsManager *settingsManager, QWidget *parent)
    : QDialog(parent)
    , m_settingsManager(settingsManager)
{
    setWindowTitle(&quot;Settings&quot;);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    tabWidget = new QTabWidget(this);
    mainLayout-&gt;addWidget(tabWidget);

    // General Tab
    QWidget *generalTab = new QWidget();
    QVBoxLayout *generalLayout = new QVBoxLayout(generalTab);

    parallelLabel = new QLabel(&quot;Parallel Tasks:&quot;);
    parallelTasksSpinBox = new QSpinBox();
    parallelTasksSpinBox-&gt;setRange(1, 32);
<<<<<<< HEAD
    parallelTasksSpinBox-&gt;setValue(m_settingsManager-&gt;parallelTasks());

    generalLayout-&gt;addWidget(parallelLabel);
    generalLayout-&gt;addWidget(parallelTasksSpinBox);

    hashLabel = new QLabel(&quot;Hash Algorithm:&quot;);
    hashAlgoComboBox = new QComboBox();
    hashAlgoComboBox-&gt;addItems({&quot;SHA256&quot;, &quot;SHA512&quot;});
<<<<<<< HEAD
    hashAlgoComboBox-&gt;setCurrentText(m_settingsManager-&gt;hashAlgo());

    generalLayout-&gt;addWidget(hashLabel);
    generalLayout-&gt;addWidget(hashAlgoComboBox);

    tabWidget-&gt;addTab(generalTab, &quot;General&quot;);

    // Drive Tab
    QWidget *driveTab = new QWidget();
    QVBoxLayout *driveLayout = new QVBoxLayout(driveTab);

    pollLabel = new QLabel(&quot;Drive Poll Interval (s):&quot;);
    pollIntervalSpinBox = new QSpinBox();
    pollIntervalSpinBox-&gt;setRange(1, 60);
<<<<<<< HEAD
    pollIntervalSpinBox-&gt;setValue(m_settingsManager-&gt;drivePollInterval());
    pollIntervalSpinBox-&gt;setSuffix(&quot; s&quot;);

    driveLayout-&gt;addWidget(pollLabel);
    driveLayout-&gt;addWidget(pollIntervalSpinBox);

    tabWidget-&gt;addTab(driveTab, &quot;Drive&quot;);

<<<<<<< HEAD
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    saveButton = new QPushButton(&quot;Save&quot;);
    resetButton = new QPushButton(&quot;Reset to Defaults&quot;);
    QPushButton *cancelButton = new QPushButton(&quot;Cancel&quot;);

    buttonLayout-&gt;addWidget(saveButton);
    buttonLayout-&gt;addWidget(resetButton);
    buttonLayout-&gt;addWidget(cancelButton);
    mainLayout-&gt;addLayout(buttonLayout);

    connect(saveButton, &amp;QPushButton::clicked, this, &amp;SettingsDialog::saveSettings);
    connect(resetButton, &amp;QPushButton::clicked, this, &amp;SettingsDialog::resetSettings);
    connect(cancelButton, &amp;QPushButton::clicked, this, &amp;QDialog::reject);
}

void SettingsDialog::saveSettings() {
    // Save to ConfigManager
    // Note: parallelTasks and pollInterval would need getters/setters added to ConfigManager
    m_configManager-&gt;setHashAlgorithm(hashAlgoComboBox-&gt;currentText());
    m_configManager-&gt;saveAllSettings();
    accept();
}

void SettingsDialog::resetSettings() {
    parallelTasksSpinBox-&gt;setValue(4);
    hashAlgoComboBox-&gt;setCurrentText(&quot;SHA256&quot;);
    pollIntervalSpinBox-&gt;setValue(5);
}

void SettingsDialog::accept() {
    saveSettings();
=======
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout-&gt;addWidget(buttonBox);

    connect(buttonBox, &amp;QDialogButtonBox::accepted, this, &amp;QDialog::accept);
    connect(buttonBox, &amp;QDialogButtonBox::rejected, this, &amp;QDialog::reject);

    connect(m_settingsManager, &amp;SettingsManager::settingsChanged, this, &amp;QDialog::accept);
}

void SettingsDialog::accept() {
    m_settingsManager-&gt;setParallelTasks(parallelTasksSpinBox-&gt;value());
    m_settingsManager-&gt;setHashAlgo(hashAlgoComboBox-&gt;currentText());
    m_settingsManager-&gt;setDrivePollInterval(pollIntervalSpinBox-&gt;value());
    QDialog::accept();
}