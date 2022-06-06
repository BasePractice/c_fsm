cmake_minimum_required(VERSION 3.12)
project(fsm C)

set(CMAKE_C_STANDARD 90)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(${PROJECT_NAME}_bulk fsm_bulk_main.c fsm_bulk.c fsm_bulk.h)
add_executable(${PROJECT_NAME}_insert fsm_insert.c fsm_insert.h fsm_insert_main.c)
add_executable(${PROJECT_NAME}_delay fsm_delay.c fsm_delay.h fsm_delay_main.c)
add_executable(${PROJECT_NAME}_coin fsm_coin.c fsm_coin.h fsm_coin_main.c)
add_executable(${PROJECT_NAME}_welding fsm_welding.c fsm_welding.h fsm_welding_main.c)
add_executable(${PROJECT_NAME}_transport transport_loader.c transport_loader.h transport_loader_main.c)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/simulate.bulk.t DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (GUI)
    include_directories(gui)

    add_executable(${PROJECT_NAME}_transport_ui gui/transport_loader_gui.c transport_loader.c transport_loader.h)
    set_property(TARGET ${PROJECT_NAME}_transport_ui PROPERTY C_STANDARD 99)
    target_link_libraries(${PROJECT_NAME}_transport_ui PUBLIC raylib)
    target_compile_definitions(${PROJECT_NAME}_transport_ui PUBLIC -DPLATFORM_DESKTOP -DGL_SILENCE_DEPRECATION)
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
    pvs_studio_add_target(TARGET ${PROJECT_NAME}_transport_loader.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}_transport_loader
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}_transport_loader.err)
endif ()
