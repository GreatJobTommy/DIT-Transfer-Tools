#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
<<<<<<< HEAD
#include <QPushButton>

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
