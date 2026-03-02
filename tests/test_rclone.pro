QT += core testlib
CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = test_rclone
TEMPLATE = app

SOURCES += test_rclone.cpp \
    ../src/TransferTask.cpp \
    ../src/SettingsManager.cpp

HEADERS += \
    ../src/TransferTask.h \
    ../src/SettingsManager.h

INCLUDEPATH += ../src