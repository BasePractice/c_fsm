cmake_minimum_required(VERSION 3.12)
project(clever_ant C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_CXX_STANDARD 20)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(${PROJECT_NAME} ant.c ant.h clever_ant.c)
add_executable(${PROJECT_NAME}_gen clever_ant_gen.cpp)

add_library(${PROJECT_NAME}_gen_Library STATIC ant.c ant.h clever_ant_gen.c )
target_link_libraries(${PROJECT_NAME}_gen PUBLIC ${PROJECT_NAME}_gen_Library)