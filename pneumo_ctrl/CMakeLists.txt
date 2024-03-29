cmake_minimum_required(VERSION 3.12)
project(pneumo C)

set(CMAKE_C_STANDARD 90)

add_definitions(-D_CRT_SECURE_NO_WARNINGS -DPNEUMO_DEBUG=1 -DPNEUMO_SIMULATE=1)
add_executable(${PROJECT_NAME} pneumo_ctrl.c pneumo_ctrl.h pneumo_ctrl_main.c)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/simulate.pneumo.t DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE common)

if (ANALYZE_ENABLE)
    pvs_studio_add_target(TARGET ${PROJECT_NAME}.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}.err)
endif ()
