cmake_minimum_required(VERSION 3.12)
project(common C)

set(CMAKE_C_STANDARD 90)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_library(${PROJECT_NAME} STATIC common.c common.h )
add_executable(${PROJECT_NAME}_string common_string_main.c)
target_link_libraries(${PROJECT_NAME}_string PRIVATE ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC .)

add_executable(${PROJECT_NAME}_tests tests.cpp)
set_property(TARGET ${PROJECT_NAME}_tests PROPERTY CXX_STANDARD 23)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/simulate.common_string.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (ANALYZE_ENABLE)
    pvs_studio_add_target(TARGET ${PROJECT_NAME}_string.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}_string
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}_string.err)
endif ()

