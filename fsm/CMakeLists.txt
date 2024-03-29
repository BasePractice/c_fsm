cmake_minimum_required(VERSION 3.12)
project(fsm C)

set(CMAKE_C_STANDARD 99)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(${PROJECT_NAME}_bulk fsm_bulk_main.c fsm_bulk.c fsm_bulk.h)
add_executable(${PROJECT_NAME}_insert fsm_insert.c fsm_insert.h fsm_insert_main.c)
add_executable(${PROJECT_NAME}_delay fsm_delay.c fsm_delay.h fsm_delay_main.c)
add_executable(${PROJECT_NAME}_coin fsm_coin.c fsm_coin.h fsm_coin_main.c)
add_executable(${PROJECT_NAME}_welding fsm_welding.c fsm_welding.h fsm_welding_main.c)
add_executable(${PROJECT_NAME}_transport transport_loader.c transport_loader.h transport_loader_main.c)

target_link_libraries(${PROJECT_NAME}_transport PRIVATE FSM::Common)
target_link_libraries(${PROJECT_NAME}_bulk PRIVATE FSM::Common)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/simulate.bulk.t DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/factory.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
if (GUI)
    include_directories(gui)

    add_executable(${PROJECT_NAME}_transport_ui gui/transport_loader_gui.cpp transport_loader.c transport_loader.h)
    set_property(TARGET ${PROJECT_NAME}_transport_ui PROPERTY CXX_STANDARD 20)
    set_property(TARGET ${PROJECT_NAME}_transport_ui PROPERTY C_STANDARD 99)
    #    target_compile_options(${PROJECT_NAME}_transport_ui PRIVATE -Wfatal-errors -pedantic -Wall -Wextra -Werror -fno-omit-frame-pointer)
    target_compile_options(${PROJECT_NAME}_transport_ui PRIVATE -Wenum-compare)
    #    target_link_options(${PROJECT_NAME}_transport_ui PRIVATE -fsanitize=undefined -fsanitize-undefined-trap-on-error)
    target_compile_definitions(${PROJECT_NAME}_transport_ui PRIVATE -DPLATFORM_DESKTOP)
    target_link_libraries(${PROJECT_NAME}_transport_ui PUBLIC raylib)
    target_link_libraries(${PROJECT_NAME}_transport_ui PRIVATE FSM::Common)
    target_compile_definitions(${PROJECT_NAME}_transport_ui PUBLIC -DPLATFORM_DESKTOP -DGL_SILENCE_DEPRECATION -D_DEBUG)

    add_executable(${PROJECT_NAME}_transport_samples_ui gui/transport_samples.cpp)
    set_property(TARGET ${PROJECT_NAME}_transport_samples_ui PROPERTY CXX_STANDARD 20)
    target_link_libraries(${PROJECT_NAME}_transport_samples_ui PUBLIC raylib)
    target_link_libraries(${PROJECT_NAME}_transport_samples_ui PRIVATE FSM::Common)
    target_compile_definitions(${PROJECT_NAME}_transport_samples_ui PUBLIC -DPLATFORM_DESKTOP -DGL_SILENCE_DEPRECATION)
endif ()

if (ANALYZE_ENABLE)
    pvs_studio_add_target(TARGET ${PROJECT_NAME}_bulk.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}_bulk
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}_bulk.err)
    pvs_studio_add_target(TARGET ${PROJECT_NAME}_insert.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}_insert
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}_insert.err)
    pvs_studio_add_target(TARGET ${PROJECT_NAME}_delay.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}_delay
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}_delay.err)
    pvs_studio_add_target(TARGET ${PROJECT_NAME}_coin.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}_coin
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}_coin.err)
    pvs_studio_add_target(TARGET ${PROJECT_NAME}_welding.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}_welding
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}_welding.err)
endif ()

if (POSITION_IMPLEMENT)
    if (GUI)
        if (UNIX)
            target_include_directories(${PROJECT_NAME}_transport_ui PRIVATE /Users/pastor/github/fsmvm/gen/c /Users/pastor/github/fsmvm/sdk/c)
            target_sources(${PROJECT_NAME}_transport_ui PRIVATE /Users/pastor/github/fsmvm/gen/c/position.h /Users/pastor/github/fsmvm/gen/c/position.c)
        else ()
            target_include_directories(${PROJECT_NAME}_transport_ui PRIVATE E:/GitHub/fsmvm/gen/c E:/GitHub/fsmvm/sdk/c)
            target_sources(${PROJECT_NAME}_transport_ui PRIVATE E:/GitHub/fsmvm/gen/c/position.h E:/GitHub/fsmvm/gen/c/position.c)
        endif ()
    endif ()
else ()
    target_compile_definitions(${PROJECT_NAME}_transport_ui PRIVATE -DPOSITION_IMPLEMENT_STUBS)
endif ()
target_compile_definitions(${PROJECT_NAME}_transport_ui PRIVATE -D_DEBUG)

