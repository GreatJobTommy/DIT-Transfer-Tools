QT += core gui widgets network concurrent

CONFIG += c++17

TARGET = DIT-Transfer-Tools
TEMPLATE = app

SOURCES += \
    main.cpp \
    src/MainWindow.cpp \
    src/TransferTask.cpp \
    src/HashManager.cpp \
    src/QueueManager.cpp \
    src/DriveMonitor.cpp

HEADERS += \
    src/MainWindow.h \
    src/TransferTask.h \
    src/HashManager.h \
    src/QueueManager.h \
    src/DriveMonitor.h

FORMS += \
    src/MainWindow.ui

# Logs
LOG_FILES += transfer_log.txt detailed.log

# Dokumentation
# DIT-Transfer-Tools: Qt/C++ Anwendung für sicheren Dateitransfer mit Chunk-Copy,
# Live SHA256 Hashing, Queue-Management (aktiv/wartend), Drive-Reconnect,
# Logging (transfer_log.txt + detailed.log) und Einstellungen.
# Modular aufgebaut, max 500 Zeilen pro Datei.
# Features:
# - Chunk-basierter Copy mit Resume
# - Live SHA256 Hash-Berechnung während Transfer
# - Queue mit Status (aktiv/wartend/pausiert)
# - Automatisches Drive-Reconnect bei USB/Laufwerk-Verlust
# - Detailliertes Logging
# - GUI mit Qt Widgets für Settings und Status