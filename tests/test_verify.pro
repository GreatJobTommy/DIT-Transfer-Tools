QT += core gui widgets printsupport testlib

TARGET = test_verify
TEMPLATE = app

SOURCES += test_verify.cpp \
    ../src/VerifyWizard.cpp

HEADERS += ../src/VerifyWizard.h

INCLUDEPATH += ../src

CONFIG += console
CONFIG -= app_bundle