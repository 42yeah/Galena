cmake_minimum_required(VERSION 3.10.0)

project(Galena)

set(CMAKE_CXX_STANDARD 17)

option(GALENA_BUILD_SAMPLES "Whether Galena should build samples" ON)
option(GALENA_BUILD_GAME "Whether to build the game part of Galena" ON)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/engine")

if (EMSCRIPTEN)
    # As of right now, the game only supports WASM build
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/game")
endif ()
