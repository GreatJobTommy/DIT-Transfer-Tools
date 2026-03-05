QT += core gui widgets

CONFIG += c++17

TARGET = DIT-Transfer-Tools
TEMPLATE = app

SOURCES += \
    main.cpp \
    src/MainWindow.cpp \
    src/RcloneRemoteDialog.cpp \
    src/TransferTask.cpp \
    src/QueueManager.cpp \
    src/DriveMonitor.cpp \
    src/ParallelManager.cpp \
    src/AddTaskDialog.cpp \
    src/ProgressMonitor.cpp \
    src/SettingsManager.cpp \
    src/LogsDockWidget.cpp \
    src/MainWindow_slots_rclone.cpp \
    src/RcloneRemoteDialog.cpp

HEADERS += \
    src/MainWindow.h \
    src/RcloneRemoteDialog.h \
    src/TransferTask.h \
    src/QueueManager.h \
    src/DriveMonitor.h \
    src/ParallelManager.h \
    src/AddTaskDialog.h \
    src/ProgressMonitor.h \
    src/SettingsManager.h \
    src/LogsDockWidget.h

FORMS += \
    src/AddTaskDialog.ui

# Tests
test_ui_clarity.target = test_ui_clarity
test_ui_clarity.commands = cd tests && $(QMAKE) test_ui_clarity.pro && $(MAKE) && ./test_ui_clarity
test_reconnect.target = test_reconnect
test_reconnect.commands = cd tests && $(QMAKE) test_reconnect.pro && $(MAKE) && ./test_reconnect
test_avgs.target = test_avgs
test_avgs.commands = cd tests && $(QMAKE) test_avgs.pro && $(MAKE) && ./test_avgs
test_logs.target = test_logs
test_logs.commands = cd tests && $(QMAKE) test_logs.pro && $(MAKE) && ./test_logs
QMAKE_EXTRA_TARGETS += test_ui_clarity test_reconnect test_avgs test_logs
