#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QSlider>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>

class SettingsManager;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(SettingsManager* settings, QWidget* parent = nullptr);
    ~SettingsDialog();

private slots:
    void onSaveClicked();
    void onLoadClicked();
    void onResetClicked();
    void onSearchTextChanged(const QString& text);
    void onAddPresetClicked();
    void onRemovePresetClicked();
    void onPresetDoubleClicked(QListWidgetItem* item);
    void toggleSection(QGroupBox* groupBox);

private:
    void setupUI();
    void loadSettings();
    void saveSettings();
    void filterSections(const QString& text);

    SettingsManager* m_settings;

    QScrollArea* m_scrollArea;
    QWidget* m_contentWidget;
    QVBoxLayout* m_layout;

    // Sections
    QGroupBox* m_queueSection;
    QGroupBox* m_hashSection;
    QGroupBox* m_transferSection;
    QGroupBox* m_drivesSection;
    QGroupBox* m_uiSection;
    QGroupBox* m_logsSection;
    QGroupBox* m_presetsSection;
    QGroupBox* m_advancedSection;

    // Widgets
    QSlider* m_queueSlider;
    QLabel* m_queueLabel;

    QComboBox* m_hashCombo;

    QSlider* m_chunkSlider;
    QLabel* m_chunkLabel;
    QCheckBox* m_chunkAutoCheck;

    QSlider* m_scanSlider;
    QLabel* m_scanLabel;

    QCheckBox* m_themeCheck;

    QComboBox* m_logsCombo;

    QListWidget* m_presetsList;
    QLineEdit* m_presetEdit;
    QPushButton* m_addPresetBtn;
    QPushButton* m_removePresetBtn;

    QCheckBox* m_rcloneCheck;

    // Bottom buttons
    QPushButton* m_saveJsonBtn;
    QPushButton* m_loadJsonBtn;
    QPushButton* m_resetBtn;
    QPushButton* m_okBtn;
    QPushButton* m_cancelBtn;

    QLineEdit* m_searchEdit;
};

#endif // SETTINGSDIALOG_H