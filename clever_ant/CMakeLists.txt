cmake_minimum_required(VERSION 3.12)
project(clever_ant C)

set(CMAKE_C_STANDARD 90)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(${PROJECT_NAME} clever_ant.c)
add_executable(${PROJECT_NAME}_gen clever_ant_gen.c)
