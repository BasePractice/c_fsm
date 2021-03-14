cmake_minimum_required(VERSION 3.12)
project(clever_ant C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_CXX_STANDARD 20)

include_directories(. bloom)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(${PROJECT_NAME} clever_ant.c)
add_executable(${PROJECT_NAME}_seq_tests sequence_tests.c)
add_executable(${PROJECT_NAME}_gen clever_ant_gen_main.c)

add_library(${PROJECT_NAME}_gen_Library STATIC sequence.c sequence.h ant.c ant.h clever_ant_gen.c)
target_link_libraries(${PROJECT_NAME}_gen PUBLIC ${PROJECT_NAME}_gen_Library)
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_gen_Library)
target_link_libraries(${PROJECT_NAME}_seq_tests PUBLIC ${PROJECT_NAME}_gen_Library)

if (GUI)
    if (UNIX)
        find_package(raylib 2.0 REQUIRED /usr/local)
    else ()
        include_directories(D:/msys64/usr/local/include)
        link_directories(D:/msys64/usr/local/lib)
    endif ()
    include_directories(gui)

    add_executable(${PROJECT_NAME}_gen_ui gui/clever_ant_gen_gui.c)
    set_property(TARGET ${PROJECT_NAME}_gen_ui PROPERTY C_STANDARD 99)
    target_link_libraries(${PROJECT_NAME}_gen_ui PUBLIC ${PROJECT_NAME}_gen_Library raylib)
    target_compile_definitions(${PROJECT_NAME}_gen_ui PUBLIC -DPLATFORM_DESKTOP -DGL_SILENCE_DEPRECATION)
endif ()
