cmake_minimum_required(VERSION 3.10.0)

project(Galena)

option(GALENA_BUILD_SAMPLES "Whether Galena should build samples" ON)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/engine")
