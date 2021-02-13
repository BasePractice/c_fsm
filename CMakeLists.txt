cmake_minimum_required(VERSION 3.12)
project(c_fsm C)

set(CMAKE_C_STANDARD 90)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(c_fsm src_c/fsm_main.c src_c/fsm.c src_c/fsm.h)
add_executable(c_life src_c/game_life.c)
add_executable(c_clever_ant src_c/clever_ant.c)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/simulate.input.t DESTINATION ${CMAKE_CURRENT_BINARY_DIR})