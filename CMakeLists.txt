cmake_minimum_required(VERSION 3.12)
project(c_fsm C CXX)

set(CMAKE_C_STANDARD 90)
set(CMAKE_CXX_STANDARD 20)

include(PVS-Studio.cmake)
#-DCMAKE_EXPORT_COMPILE_COMMANDS=ON
#pvs-studio-analyzer credentials PVS-Studio Free FREE-FREE-FREE-FREE
option(MEMORY_DEBUG "Debugging allocate memoty" OFF)
option(ANALYZE_ENABLE "Включение PVS анализатора кода" OFF)

include_directories(include)

if (MEMORY_DEBUG)
    add_definitions(-DMEMORY_DEBUG)
endif ()

if (GUI)
    if (UNIX)
        find_package(raylib 2.0 REQUIRED /usr/local)
    else ()
        include_directories(D:/msys64/usr/local/include)
        link_directories(D:/msys64/usr/local/lib)
    endif ()
endif ()

add_subdirectory(common)
add_subdirectory(clever_ant)
add_subdirectory(forth)
add_subdirectory(fsm)
add_subdirectory(game_life)
add_subdirectory(math_lang)
add_subdirectory(microc)
add_subdirectory(pneumo_ctrl)
add_subdirectory(regexp)
add_subdirectory(turing)