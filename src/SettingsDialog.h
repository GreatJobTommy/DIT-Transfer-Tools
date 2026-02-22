#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include &lt;QDialog&gt;
#include &lt;QTabWidget&gt;
#include &lt;QLabel&gt;
#include &lt;QSpinBox&gt;
#include &lt;QComboBox&gt;
#include &lt;QPushButton&gt;

class ConfigManager;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(ConfigManager *configManager, QWidget *parent = nullptr);

private slots:
    void saveSettings();
    void resetSettings();
    void accept() override;

private:
    ConfigManager *m_configManager;

    QTabWidget *tabWidget;
    QLabel *parallelLabel;
    QSpinBox *parallelTasksSpinBox;
    QLabel *hashLabel;
    QComboBox *hashAlgoComboBox;
    QLabel *pollLabel;
    QSpinBox *pollIntervalSpinBox;
    QPushButton *saveButton;
    QPushButton *resetButton;
};

#endif // SETTINGSDIALOG_H
