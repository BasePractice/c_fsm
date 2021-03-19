cmake_minimum_required(VERSION 3.12)
project(life C)

set(CMAKE_C_STANDARD 90)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_library(${PROJECT_NAME}_Library STATIC game_life.c)
add_executable(${PROJECT_NAME}_main game_life_main.c)
target_link_libraries(${PROJECT_NAME}_main PUBLIC ${PROJECT_NAME}_Library)

if (GUI)
    include_directories(gui)

    add_executable(${PROJECT_NAME}_ui gui/game_life_gui.c)
    set_property(TARGET ${PROJECT_NAME}_ui PROPERTY C_STANDARD 99)
    target_link_libraries(${PROJECT_NAME}_ui PUBLIC ${PROJECT_NAME}_Library raylib)
    target_compile_definitions(${PROJECT_NAME}_ui PUBLIC -DPLATFORM_DESKTOP -DGL_SILENCE_DEPRECATION)
endif ()
