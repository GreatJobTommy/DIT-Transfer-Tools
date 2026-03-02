cmake_minimum_required(VERSION 3.16)
project(DIT-Transfer-Tools VERSION 4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(DIT-Transfer-Tools
    main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/TransferTask.cpp
    src/TransferTask.h
    src/QueueManager.cpp
    src/QueueManager.h
    src/AddTaskDialog.cpp
    src/AddTaskDialog.h
    src/AddTaskDialog.ui
    src/MainWindow.ui
    src/LogsDockWidget.h
    src/LogsDockWidget.cpp
    src/ParallelManager.cpp
    src/ParallelManager.h
    src/DriveMonitor.cpp
    src/DriveMonitor.h
    src/ProgressMonitor.cpp
    src/ProgressMonitor.h
    src/ErrorManager.cpp
    src/ErrorManager.h
    src/SettingsManager.cpp
    src/SettingsManager.h
    src/NotificationManager.cpp
    src/NotificationManager.h
    src/DragDropList.h
)

target_link_libraries(DIT-Transfer-Tools Qt6::Core Qt6::Widgets)

# Tests
enable_testing()
add_executable(test_queuemanager tests/test_queuemanager.cpp src/TransferTask.cpp src/QueueManager.cpp)
target_link_libraries(test_queuemanager Qt6::Core Qt6::Test)
add_test(NAME TestQueueManager COMMAND test_queuemanager)