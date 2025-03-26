cmake_minimum_required(VERSION 3.10)

project(control LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Folder structure
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(HEADERS_DIR "${CMAKE_SOURCE_DIR}/headers")

# Source files
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# Qt 6 setup
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

# Other dependencies
find_package(ur_rtde REQUIRED)
find_package(OpenCV REQUIRED)

# Ensure MOC runs on MainWindow.hpp
qt6_wrap_cpp(MOC_SOURCES ${HEADERS_DIR}/MainWindow.hpp)
list(APPEND SOURCES ${MOC_SOURCES})

# Executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include paths
target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${HEADERS_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    ur_rtde::rtde
    ${OpenCV_LIBS}
)
