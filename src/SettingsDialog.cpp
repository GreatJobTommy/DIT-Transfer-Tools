#include &quot;SettingsDialog.h&quot;
#include &quot;ConfigManager.h&quot;
#include &lt;QVBoxLayout&gt;
#include &lt;QHBoxLayout&gt;
#include &lt;QDialogButtonBox&gt;

SettingsDialog::SettingsDialog(ConfigManager *configManager, QWidget *parent)
    : QDialog(parent)
    , m_configManager(configManager)
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
    parallelTasksSpinBox-&gt;setValue(4); // Default

    generalLayout-&gt;addWidget(parallelLabel);
    generalLayout-&gt;addWidget(parallelTasksSpinBox);

    hashLabel = new QLabel(&quot;Hash Algorithm:&quot;);
    hashAlgoComboBox = new QComboBox();
    hashAlgoComboBox-&gt;addItems({&quot;SHA256&quot;, &quot;SHA512&quot;});
    hashAlgoComboBox-&gt;setCurrentText(m_configManager-&gt;getHashAlgorithm());

    generalLayout-&gt;addWidget(hashLabel);
    generalLayout-&gt;addWidget(hashAlgoComboBox);

    tabWidget-&gt;addTab(generalTab, &quot;General&quot;);

    // Drive Tab
    QWidget *driveTab = new QWidget();
    QVBoxLayout *driveLayout = new QVBoxLayout(driveTab);

    pollLabel = new QLabel(&quot;Drive Poll Interval (s):&quot;);
    pollIntervalSpinBox = new QSpinBox();
    pollIntervalSpinBox-&gt;setRange(1, 60);
    pollIntervalSpinBox-&gt;setValue(5); // Default
    pollIntervalSpinBox-&gt;setSuffix(&quot; s&quot;);

    driveLayout-&gt;addWidget(pollLabel);
    driveLayout-&gt;addWidget(pollIntervalSpinBox);

    tabWidget-&gt;addTab(driveTab, &quot;Drive&quot;);

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
    QDialog::accept();
}
