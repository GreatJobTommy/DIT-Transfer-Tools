QT += core gui widgets testlib
CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = test_logs
TEMPLATE = app

SOURCES += test_logs.cpp \
    ../src/LogsDockWidget.cpp \
    ../src/ErrorManager.cpp \
    ../src/SettingsManager.cpp

HEADERS += \
    ../src/LogsDockWidget.h \
    ../src/ErrorManager.h \
    ../src/SettingsManager.h

INCLUDEPATH += ../src