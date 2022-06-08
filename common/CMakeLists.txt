cmake_minimum_required(VERSION 3.12)
project(common C)

set(CMAKE_C_STANDARD 90)

add_library(sha3 STATIC sha3.c sha3.h)
target_include_directories(sha3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(sha3 PUBLIC -fPIC)
add_library(FSM::SHA3 ALIAS sha3)


add_library(cJSON STATIC cJSON.c cJSON.h cJSON_Utils.c cJSON_Utils.h)
target_include_directories(cJSON PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(cJSON PUBLIC -fPIC)
target_compile_definitions(cJSON PUBLIC -DCJSON_HIDE_SYMBOLS)
add_library(FSM::cJSON ALIAS cJSON)

add_library(common STATIC c_logger.c c_logger.h u_memory.c u_memory.h configuration.c configuration.h common.c base64.c)
set_property(TARGET common PROPERTY POSITION_INDEPENDENT_CODE TRUE)
target_compile_options(common PUBLIC -fPIC)
target_link_libraries(common PUBLIC FSM::cJSON FSM::SHA3)

add_executable(${PROJECT_NAME}_string common_string_main.c)
target_link_libraries(${PROJECT_NAME}_string PRIVATE ${PROJECT_NAME})

if (UNIX)
    target_link_libraries(common PUBLIC dl)
endif ()

if (ANDROID)
    find_library(loglib log PATH "${ANDROID_NDK}/platforms/${ANDROID_PLATFORM}/arch-${ANDROID_SYSROOT_ABI}/usr/lib/")

    target_link_libraries(common PUBLIC ${loglib})
endif ()

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(FSM::Common ALIAS common)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/simulate.common_string.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (ANALYZE_ENABLE)
    pvs_studio_add_target(TARGET ${PROJECT_NAME}_string.analyze ALL
            OUTPUT FORMAT errorfile
            ANALYZE ${PROJECT_NAME}_string
            MODE ${ANALYZE_MODE}
            C_FLAGS -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include
            LOG ${PROJECT_NAME}_string.err)
endif ()

