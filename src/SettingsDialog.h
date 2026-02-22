#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include &lt;QDialog&gt;
#include &lt;QTabWidget&gt;
#include &lt;QLabel&gt;
#include &lt;QSpinBox&gt;
#include &lt;QComboBox&gt;

class SettingsManager;

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
};

#endif // SETTINGSDIALOG_H
