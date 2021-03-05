cmake_minimum_required(VERSION 3.12)
project(pneumo C)

set(CMAKE_C_STANDARD 90)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(${PROJECT_NAME} pneumo_ctrl.c pneumo_ctrl.h pneumo_ctrl_main.c)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/simulate.pneumo.t DESTINATION ${CMAKE_CURRENT_BINARY_DIR})