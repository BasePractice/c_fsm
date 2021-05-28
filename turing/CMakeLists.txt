cmake_minimum_required(VERSION 3.12)
project(turung C)

set(CMAKE_C_STANDARD 90)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(${PROJECT_NAME} turing.c turing.h turing_main.c)
add_executable(${PROJECT_NAME}_direct_sharp turing_direct_sharp.c)
add_executable(${PROJECT_NAME}_direct_remove turing_direct_remove.c)

if (ANALYZE_ENABLE)
    pvs_studio_add_target(TARGET ${PROJECT_NAME}.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}.err)
    pvs_studio_add_target(TARGET ${PROJECT_NAME}_direct_sharp.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}_direct_sharp
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}_direct_sharp.err)
    pvs_studio_add_target(TARGET ${PROJECT_NAME}_direct_remove.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}_direct_remove
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}_direct_remove.err)
endif ()
