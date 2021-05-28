cmake_minimum_required(VERSION 3.12)
project(turung C)

set(CMAKE_C_STANDARD 90)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(${PROJECT_NAME} turing.c turing.h turing_main.c)
add_executable(${PROJECT_NAME}_direct_sharp turing_direct_sharp.c)
add_executable(${PROJECT_NAME}_direct_remove turing_direct_remove.c)
