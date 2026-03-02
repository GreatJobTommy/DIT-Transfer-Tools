QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  \
    ../src/TransferTask.cpp \
    test_multi_target.cpp

HEADERS += \
    ../src/TransferTask.h

INCLUDEPATH += ../src