cmake_minimum_required(VERSION 3.16)
project(DIT-Transfer-Tools VERSION 2.0 LANGUAGES CXX)

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
    src/ParallelManager.cpp
    src/ParallelManager.h
    src/HashManager.cpp
    src/HashManager.h
    src/DriveMonitor.cpp
    src/DriveMonitor.h
    src/SettingsManager.cpp
    src/SettingsManager.h
    src/ProgressMonitor.cpp
    src/ProgressMonitor.h
    src/ErrorManager.cpp
    src/ErrorManager.h
)

target_link_libraries(DIT-Transfer-Tools Qt6::Core Qt6::Widgets)

# Tests
enable_testing()
add_executable(test_queuemanager tests/test_queuemanager.cpp src/TransferTask.cpp src/QueueManager.cpp)
target_link_libraries(test_queuemanager Qt6::Core Qt6::Test)
add_test(NAME TestQueueManager COMMAND test_queuemanager)

add_executable(test_addtask tests/test_addtask.cpp src/AddTaskDialog.cpp src/TransferTask.cpp src/ParallelManager.cpp)
target_link_libraries(test_addtask Qt6::Core Qt6::Widgets Qt6::Test)
add_test(NAME TestAddTask COMMAND test_addtask)

add_executable(test_configmanager tests/test_configmanager.cpp)
target_link_libraries(test_configmanager Qt6::Core Qt6::Test)
add_test(NAME TestConfigManager COMMAND test_configmanager)

add_executable(test_drivemonitor tests/test_drivemonitor.cpp)
target_link_libraries(test_drivemonitor Qt6::Core Qt6::Test)
add_test(NAME TestDriveMonitor COMMAND test_drivemonitor)

add_executable(test_errorhandling tests/test_errorhandling.cpp)
target_link_libraries(test_errorhandling Qt6::Core Qt6::Test)
add_test(NAME TestErrorHandling COMMAND test_errorhandling)

add_executable(test_ui tests/test_ui.cpp src/QueueManager.cpp src/QueueManager.h src/TransferTask.cpp src/TransferTask.h src/MainWindow.cpp src/MainWindow.h src/AddTaskDialog.cpp src/AddTaskDialog.h src/AddTaskDialog.ui src/ParallelManager.cpp src/ParallelManager.h src/HashManager.cpp src/HashManager.h src/DriveMonitor.cpp src/DriveMonitor.h src/SettingsManager.cpp src/SettingsManager.h src/ProgressMonitor.cpp src/ProgressMonitor.h src/ErrorManager.cpp src/ErrorManager.h)
target_link_libraries(test_ui Qt6::Core Qt6::Widgets Qt6::Test)
add_test(NAME TestUI COMMAND test_ui)