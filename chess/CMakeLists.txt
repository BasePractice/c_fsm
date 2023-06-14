cmake_minimum_required(VERSION 3.12)
project(chess C)

set(CMAKE_C_STANDARD 99)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(${PROJECT_NAME}_main chess_main.c)

target_link_libraries(${PROJECT_NAME}_main PRIVATE FSM::Common)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
if (GUI)
    include_directories(gui)

    add_executable(${PROJECT_NAME}_ui gui/chess_gui.cpp)
    set_property(TARGET ${PROJECT_NAME}_ui PROPERTY CXX_STANDARD 20)
    set_property(TARGET ${PROJECT_NAME}_ui PROPERTY C_STANDARD 99)
    target_compile_options(${PROJECT_NAME}_ui PRIVATE -Wenum-compare)
    target_compile_definitions(${PROJECT_NAME}_ui PRIVATE -DPLATFORM_DESKTOP)
    target_link_libraries(${PROJECT_NAME}_ui PUBLIC raylib)
    target_link_libraries(${PROJECT_NAME}_ui PRIVATE FSM::Common)
    target_compile_definitions(${PROJECT_NAME}_ui PUBLIC -DPLATFORM_DESKTOP -DGL_SILENCE_DEPRECATION -D_DEBUG)
endif ()

if (ANALYZE_ENABLE)
    pvs_studio_add_target(TARGET ${PROJECT_NAME}_main.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}_main
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}_main.err)
endif ()

if (POSITION_IMPLEMENT)
    if (GUI)
        if (UNIX)
            target_include_directories(${PROJECT_NAME}_ui PRIVATE /Users/pastor/github/fsmvm/gen/c /Users/pastor/github/fsmvm/sdk/c)
            target_sources(${PROJECT_NAME}_ui PRIVATE /Users/pastor/github/fsmvm/gen/c/position.h /Users/pastor/github/fsmvm/gen/c/position.c)
        else ()
            target_include_directories(${PROJECT_NAME}_ui PRIVATE E:/GitHub/fsmvm/gen/c E:/GitHub/fsmvm/sdk/c)
            target_sources(${PROJECT_NAME}_ui PRIVATE E:/GitHub/fsmvm/gen/c/position.h E:/GitHub/fsmvm/gen/c/position.c)
        endif ()
    endif ()
else ()
    target_compile_definitions(${PROJECT_NAME}_ui PRIVATE -DPOSITION_IMPLEMENT_STUBS)
endif ()
target_compile_definitions(${PROJECT_NAME}_ui PRIVATE -D_DEBUG)

