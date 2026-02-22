QT += core gui widgets network concurrent

CONFIG += c++17

TARGET = DIT
TEMPLATE = app

SOURCES += \
        src/AddTaskDialog.cpp \
    main.cpp \
    src/MainWindow.cpp \
    src/TransferTask.cpp \
    src/HashManager.cpp \
    src/QueueManager.cpp \
    src/DriveMonitor.cpp

HEADERS += \
        src/AddTaskDialog.h \
    src/MainWindow.h \
    src/TransferTask.h \
    src/HashManager.h \
    src/QueueManager.h \
    src/DriveMonitor.h

FORMS += \
        src/AddTaskDialog.ui \
        src/MainWindow.ui \
    src/MainWindow.ui