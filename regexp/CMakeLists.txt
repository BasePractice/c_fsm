cmake_minimum_required(VERSION 3.12)
project(regexp C)

set(CMAKE_C_STANDARD 90)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(${PROJECT_NAME}_isbn regexp_isbn.c)
add_executable(${PROJECT_NAME}_tags regexp_tags.c)
add_executable(${PROJECT_NAME}_nums_1 regexp_nums_1.c)
add_executable(${PROJECT_NAME}_nums_2 regexp_nums_2.c)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/simulate.isbn.t DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/simulate.nums.t DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (ANALYZE_ENABLE)
    pvs_studio_add_target(TARGET ${PROJECT_NAME}_isbn.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}_isbn
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}_isbn.err)
    pvs_studio_add_target(TARGET ${PROJECT_NAME}_tags.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}_tags
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}_tags.err)
    pvs_studio_add_target(TARGET ${PROJECT_NAME}_nums_1.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}_nums_1
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}_nums_1.err)
    pvs_studio_add_target(TARGET ${PROJECT_NAME}_nums_2.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}_nums_2
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}_nums_2.err)
endif ()