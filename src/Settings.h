#ifndef SETTINGS_H
#define SETTINGS_H

#include &lt;QObject&gt;

class Settings : public QObject {
public:
    static int maxParallelTransfers();
};

#endif
