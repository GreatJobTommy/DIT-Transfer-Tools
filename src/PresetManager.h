#ifndef PRESETMANAGER_H
#define PRESETMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

class PresetManager : public QObject {
    Q_OBJECT

public:
    explicit PresetManager(QObject* parent = nullptr);
    ~PresetManager();

    struct Preset {
        QString name;
        QString description;
        QJsonObject settings;
        QJsonArray tasks; // Array of task templates
    };

    bool savePreset(const Preset& preset);
    bool loadPreset(const QString& name, Preset& preset);
    QStringList getPresetNames() const;
    bool deletePreset(const QString& name);

    // Built-in presets
    void createDefaultPresets();

signals:
    void presetSaved(const QString& name);
    void presetLoaded(const QString& name);
    void presetDeleted(const QString& name);

private:
    QString getPresetsDir() const;
    QString getPresetFilePath(const QString& name) const;
};

#endif // PRESETMANAGER_H