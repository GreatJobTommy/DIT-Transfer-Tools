#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include &lt;QDialog&gt;
#include &lt;QTabWidget&gt;
#include &lt;QLabel&gt;
#include &lt;QSpinBox&gt;
#include &lt;QComboBox&gt;
<<<<<<< HEAD
#include &lt;QPushButton&gt;

class ConfigManager;
=======

class SettingsManager;
>>>>>>> origin/main

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(SettingsManager *settingsManager, QWidget *parent = nullptr);

private slots:
    void accept() override;

private:
    SettingsManager *m_settingsManager;

    QTabWidget *tabWidget;
    QLabel *parallelLabel;
    QSpinBox *parallelTasksSpinBox;
    QLabel *hashLabel;
    QComboBox *hashAlgoComboBox;
    QLabel *pollLabel;
    QSpinBox *pollIntervalSpinBox;
<<<<<<< HEAD
    QPushButton *saveButton;
    QPushButton *resetButton;
=======
>>>>>>> origin/main
};

#endif // SETTINGSDIALOG_H
