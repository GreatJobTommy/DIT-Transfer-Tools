QT += core gui widgets network concurrent charts

CONFIG += c++17

TARGET = DIT-Transfer-Tools
TEMPLATE = app

SOURCES += \
    main.cpp \
    src/MainWindow.cpp \
    src/TransferTask.cpp \
    src/QueueManager.cpp \
    src/DriveMonitor.cpp \
    src/ParallelManager.cpp \
    src/AddTaskDialog.cpp \
    src/ProgressMonitor.cpp \
    src/ErrorManager.cpp \
    src/SettingsManager.cpp \
    src/SpeedHistory.cpp

HEADERS += \
    src/MainWindow.h \
    src/TransferTask.h \
    src/QueueManager.h \
    src/DriveMonitor.h \
    src/ParallelManager.h \
    src/AddTaskDialog.h \
    src/ProgressMonitor.h \
    src/ErrorManager.h \
    src/SettingsManager.h \
    src/DragDropList.h \
    src/SpeedHistory.h

FORMS += \
    src/AddTaskDialog.ui

# Tests
test_ui_clarity.target = test_ui_clarity
test_ui_clarity.commands = cd tests && $(QMAKE) test_ui_clarity.pro && $(MAKE) && ./test_ui_clarity
<<<<<<< HEAD
test_rclone.target = test_rclone
test_rclone.commands = cd tests && $(QMAKE) test_rclone.pro && $(MAKE) && ./test_rclone
test_notify.target = test_notify
test_notify.commands = cd tests && $(QMAKE) test_notify.pro && $(MAKE) && ./test_notify
QMAKE_EXTRA_TARGETS += test_ui_clarity test_rclone test_notify test_notify

# Logs
LOG_FILES += transfer_log.txt detailed.log

# Dokumentation
# DIT-Transfer-Tools: Qt/C++ Anwendung für sicheren Dateitransfer mit Chunk-Copy,
# Live SHA256 Hashing, Queue-Management (aktiv/wartend), Drive-Reconnect,
# Logging (transfer_log.txt + detailed.log) und Einstellungen.
# Modular aufgebaut, max 500 Zeilen pro Datei.
# Features:
# - Chunk-basierter Copy mit Resume
# - Live SHA256 Hash-Berechnung während Transfer
# - Queue mit Status (aktiv/wartend/pausiert)
# - Automatisches Drive-Reconnect bei USB/Laufwerk-Verlust
# - Detailliertes Logging
# - GUI mit Qt Widgets für Settings und Status
=======
test_reconnect.target = test_reconnect
test_reconnect.commands = cd tests && $(QMAKE) test_reconnect.pro && $(MAKE) && ./test_reconnect
test_avgs.target = test_avgs
test_avgs.commands = cd tests && $(QMAKE) test_avgs.pro && $(MAKE) && ./test_avgs
QMAKE_EXTRA_TARGETS += test_ui_clarity test_reconnect test_avgs
>>>>>>> b8928050ebd865a1b6994c7da391d12e3739cc10
