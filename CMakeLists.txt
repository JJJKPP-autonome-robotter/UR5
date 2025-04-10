cmake_minimum_required(VERSION 3.10)

project(control LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(HEADERS_DIR "${CMAKE_SOURCE_DIR}/headers")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

find_package(ur_rtde REQUIRED)

find_package(OpenCV REQUIRED)

find_package(yaml-cpp REQUIRED)

find_library(SQLITE3_LIBRARY sqlite3)
find_path(SQLITE3_INCLUDE_DIR sqlite3.h)

if(NOT SQLITE3_LIBRARY OR NOT SQLITE3_INCLUDE_DIR)
    message(FATAL_ERROR "SQLite3 not found")
endif()

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PRIVATE ${HEADERS_DIR} 
            ${OpenCV_INCLUDE_DIRS}
            )

target_link_libraries(${PROJECT_NAME} 
    PRIVATE ur_rtde::rtde 
            ${OpenCV_LIBS} 
            yaml-cpp
            sqlite3
            )
