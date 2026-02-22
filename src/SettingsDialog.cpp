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
    parallelTasksSpinBox-&gt;setValue(settingsManager-&gt;parallelTasks());

    generalLayout-&gt;addWidget(parallelLabel);
    generalLayout-&gt;addWidget(parallelTasksSpinBox);

    hashLabel = new QLabel(&quot;Hash Algorithm:&quot;);
    hashAlgoComboBox = new QComboBox();
    hashAlgoComboBox-&gt;addItems({&quot;SHA256&quot;, &quot;SHA512&quot;});
    hashAlgoComboBox-&gt;setCurrentText(settingsManager-&gt;hashAlgo());

    generalLayout-&gt;addWidget(hashLabel);
    generalLayout-&gt;addWidget(hashAlgoComboBox);

    tabWidget-&gt;addTab(generalTab, &quot;General&quot;);

    // Drive Tab
    QWidget *driveTab = new QWidget();
    QVBoxLayout *driveLayout = new QVBoxLayout(driveTab);

    pollLabel = new QLabel(&quot;Drive Poll Interval (s):&quot;);
    pollIntervalSpinBox = new QSpinBox();
    pollIntervalSpinBox-&gt;setRange(1, 60);
    pollIntervalSpinBox-&gt;setValue(settingsManager-&gt;drivePollInterval());
    pollIntervalSpinBox-&gt;setSuffix(&quot; s&quot;);

    driveLayout-&gt;addWidget(pollLabel);
    driveLayout-&gt;addWidget(pollIntervalSpinBox);

    tabWidget-&gt;addTab(driveTab, &quot;Drive&quot;);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout-&gt;addWidget(buttonBox);

    connect(buttonBox, &amp;QDialogButtonBox::accepted, this, &amp;QDialog::accept);
    connect(buttonBox, &amp;QDialogButtonBox::rejected, this, &amp;QDialog::reject);

    connect(settingsManager, &amp;SettingsManager::settingsChanged, this, &amp;QDialog::accept);
}

void SettingsDialog::accept() {
    m_settingsManager-&gt;setParallelTasks(parallelTasksSpinBox-&gt;value());
    m_settingsManager-&gt;setHashAlgo(hashAlgoComboBox-&gt;currentText());
    m_settingsManager-&gt;setDrivePollInterval(pollIntervalSpinBox-&gt;value());
    QDialog::accept();
}
