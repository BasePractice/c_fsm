cmake_minimum_required(VERSION 3.12)
project(algorithms)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(${PROJECT_NAME}_tree tree.cpp)

