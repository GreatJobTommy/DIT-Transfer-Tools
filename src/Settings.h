#ifndef SETTINGS_H
#define SETTINGS_H
#include <QObject>
class Settings : public QObject {
public:
    static int parallel();
};
#endif
