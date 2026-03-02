QT += core testlib

TARGET = test_presets
TEMPLATE = app

SOURCES += test_presets.cpp \
    ../src/PresetManager.cpp

HEADERS += ../src/PresetManager.h

INCLUDEPATH += ../src

CONFIG += console
CONFIG -= app_bundle