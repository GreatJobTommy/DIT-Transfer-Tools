QT += core gui widgets network concurrent

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
    src/SettingsManager.cpp

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
    src/DragDropList.h

FORMS += \
    src/AddTaskDialog.ui

# Tests
test_ui_clarity.target = test_ui_clarity
test_ui_clarity.commands = cd tests && $(QMAKE) test_ui_clarity.pro && $(MAKE) && ./test_ui_clarity
QMAKE_EXTRA_TARGETS += test_ui_clarity
