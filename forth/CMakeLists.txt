cmake_minimum_required(VERSION 3.12)
project(forth C)

set(CMAKE_C_STANDARD 90)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(${PROJECT_NAME} forth.c forth.h forth_main.c)
