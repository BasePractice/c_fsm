cmake_minimum_required(VERSION 3.12)
project(regexp C)

set(CMAKE_C_STANDARD 90)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(${PROJECT_NAME}_isbn regexp_isbn.c)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/simulate.isbn.t DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
