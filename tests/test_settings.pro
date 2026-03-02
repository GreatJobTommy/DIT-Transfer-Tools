QT += core testlib
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = test_settings
TEMPLATE = app

SOURCES += test_settings.cpp \
    ../src/SettingsManager.cpp \
    moc_SettingsManager.cpp

HEADERS += ../src/SettingsManager.h

INCLUDEPATH += ../src