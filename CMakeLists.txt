cmake_minimum_required(VERSION 3.14)
project(c_fsm C)

set(CMAKE_C_STANDARD 90)

add_executable(c_fsm src_c/main.c src_c/fsm.c src_c/fsm.h)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/simulate.input.t DESTINATION ${CMAKE_CURRENT_BINARY_DIR})