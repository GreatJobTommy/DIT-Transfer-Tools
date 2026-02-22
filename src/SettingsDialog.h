#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H
#include <QDialog>
#include <QTabWidget>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
class ConfigManager;
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
    QPushButton *saveButton;
    QPushButton *resetButton;
};
#endif // SETTINGSDIALOG_H
