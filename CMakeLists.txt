cmake_minimum_required(VERSION 3.10)

# Set the project name
project(control LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt features (automatically runs MOC, UIC, RCC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Directories
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(HEADERS_DIR "${CMAKE_SOURCE_DIR}/headers")

# Source and header files
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE HEADERS "${HEADERS_DIR}/*.hpp") # Optional for IDE

# Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

# Other dependencies
find_package(ur_rtde REQUIRED)
find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED)

# SQLite3 (manual find)
find_library(SQLITE3_LIBRARY sqlite3)
find_path(SQLITE3_INCLUDE_DIR sqlite3.h)

if(NOT SQLITE3_LIBRARY OR NOT SQLITE3_INCLUDE_DIR)
    message(FATAL_ERROR "SQLite3 not found")
endif()

# Ensure headers with Q_OBJECT are processed by moc
set(HEADERS_WITH_QOBJECT
    ${HEADERS_DIR}/MainWindow.hpp
)

qt6_wrap_cpp(MOC_SOURCES ${HEADERS_WITH_QOBJECT})

# Define the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${MOC_SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} 
    PRIVATE 
    ${HEADERS_DIR} 
    ${OpenCV_INCLUDE_DIRS}
    ${SQLITE3_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    ur_rtde::rtde
    ${OpenCV_LIBS}
    yaml-cpp
    ${SQLITE3_LIBRARY}
)