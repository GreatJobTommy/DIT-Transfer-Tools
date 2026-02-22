#include "SettingsDialog.h"
#include "SettingsManager.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    SettingsManager settings;
    SettingsDialog dialog(&settings);
    dialog.show();

    return app.exec();
}
