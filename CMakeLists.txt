cmake_minimum_required(VERSION 3.16)
project(DIT-Transfer-Tools VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(DIT-Transfer-Tools
    main.cpp
    src/MainWindow.cpp
    src/TransferTask.cpp
    src/QueueManager.cpp
    # Add other sources as needed
)

target_link_libraries(DIT-Transfer-Tools Qt6::Core Qt6::Widgets)

# Tests
enable_testing()
add_executable(test_queuemanager tests/test_queuemanager.cpp src/TransferTask.cpp src/QueueManager.cpp)
target_link_libraries(test_queuemanager Qt6::Core Qt6::Test)
add_test(NAME TestQueueManager COMMAND test_queuemanager)