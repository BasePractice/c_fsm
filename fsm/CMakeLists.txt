cmake_minimum_required(VERSION 3.12)
project(fsm C)

set(CMAKE_C_STANDARD 90)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(${PROJECT_NAME}_bulk fsm_bulk_main.c fsm_bulk.c fsm_bulk.h)
add_executable(${PROJECT_NAME}_insert fsm_insert.c fsm_insert.h fsm_insert_main.c)
add_executable(${PROJECT_NAME}_delay fsm_delay.c fsm_delay.h fsm_delay_main.c)
add_executable(${PROJECT_NAME}_coin fsm_coin.c fsm_coin.h fsm_coin_main.c)
add_executable(${PROJECT_NAME}_welding fsm_welding.c fsm_welding.h fsm_welding_main.c)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/simulate.bulk.t DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
