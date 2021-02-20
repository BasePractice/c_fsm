cmake_minimum_required(VERSION 3.12)
project(c_fsm C)

set(CMAKE_C_STANDARD 90)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(c_fsm src_c/fsm_main.c src_c/fsm.c src_c/fsm.h)
add_executable(c_life src_c/game_life.c)
add_executable(c_clever_ant src_c/clever_ant.c)
add_executable(c_clever_ant_gen src_c/clever_ant_gen.c)
add_executable(c_classic_menu src_c/classic_menu.c)
add_executable(c_fm src_c/fm.c src_c/fm.h src_c/fm_main.c)
add_executable(c_micro src_c/micro.c)
add_executable(c_ctl src_c/ctl_main.c)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/simulate.input.t DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ctl.input.t DESTINATION ${CMAKE_CURRENT_BINARY_DIR})