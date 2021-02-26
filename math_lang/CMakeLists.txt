cmake_minimum_required(VERSION 3.12)
project(math C)

set(CMAKE_C_STANDARD 90)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(${PROJECT_NAME} math_lang.c math_lang.h math_main.c)
