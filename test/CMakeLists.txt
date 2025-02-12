cmake_minimum_required(VERSION 3.5)
project(ur_rtde_cmake_example)

find_package(ur_rtde REQUIRED)
add_executable(ur5_control main.cpp)
target_link_libraries(ur5_control PRIVATE ur_rtde::rtde)
