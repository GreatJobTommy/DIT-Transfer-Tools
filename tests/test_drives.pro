QT += core gui widgets testlib

CONFIG += c++17

TARGET = test_drives
TEMPLATE = app

SOURCES += \
    test_drives.cpp \
    ../src/MainWindow.cpp \
    ../src/TransferTask.cpp \
    ../src/QueueManager.cpp \
    ../src/DriveMonitor.cpp \
    ../src/ParallelManager.cpp \
    ../src/AddTaskDialog.cpp \
    ../src/ProgressMonitor.cpp \
    ../src/ErrorManager.cpp \
    ../src/SettingsManager.cpp \
    ../src/SpeedHistory.cpp

HEADERS += \
    ../src/MainWindow.h \
    ../src/TransferTask.h \
    ../src/QueueManager.h \
    ../src/DriveMonitor.h \
    ../src/ParallelManager.h \
    ../src/AddTaskDialog.h \
    ../src/ProgressMonitor.h \
    ../src/ErrorManager.h \
    ../src/SettingsManager.h \
    ../src/DragDropList.h \
    ../src/SpeedHistory.h

FORMS += \
    ../src/AddTaskDialog.ui

INCLUDEPATH += ../src