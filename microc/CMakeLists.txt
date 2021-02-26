cmake_minimum_required(VERSION 3.12)
project(microc C)

set(CMAKE_C_STANDARD 90)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(${PROJECT_NAME} micro.c)
add_executable(${PROJECT_NAME}_test test.c)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test.c DESTINATION ${CMAKE_CURRENT_BINARY_DIR})