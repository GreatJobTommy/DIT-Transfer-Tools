QT += core gui widgets testlib
CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = test_notify
TEMPLATE = app

SOURCES += test_notify.cpp \
    ../src/NotificationManager.cpp \
    ../src/SettingsManager.cpp

HEADERS += \
    ../src/NotificationManager.h \
    ../src/SettingsManager.h

INCLUDEPATH += ../src