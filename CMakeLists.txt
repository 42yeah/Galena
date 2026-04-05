cmake_minimum_required(VERSION 3.10.0)

project(Galena)

set(CMAKE_CXX_STANDARD 17)

option(GALENA_BUILD_SAMPLES "Whether Galena should build samples" ON)

set(GALENA_SRC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(GALENA_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(GALENA_LIBRARY_NAME "${PROJECT_NAME}")
set(GALENA_SAMPLE_WASM_DEPLOY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/html/js")

set(GALENA_SRC
    GVersion.cpp
    GEngine.cpp
    GEngineData.cpp
    GEngineResources.cpp
    GHwBuffer.cpp)

set(GALENA_EXTERNAL_PUBLIC_LIBS)
set(GALENA_EXTERNAL_PRIVATE_LIBS)

# Setup shaders

set(GALENA_SHADER_SRC_HEADER)
set(GALENA_SHADER_SRC_IMPL)

macro (process_shaders EXT)
    file(GLOB SHADER_SRCS
        "${GALENA_SRC_ROOT}/shaders/*.${EXT}")
    
    foreach (ENTRY ${SHADER_SRCS})
        file(READ "${ENTRY}" SHADER_SRC)

        set_property(DIRECTORY
            APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${ENTRY}")
        
        get_filename_component(ENTRY_NAME "${ENTRY}" NAME_WE)
        string(CONCAT GALENA_SHADER_SRC_HEADER
            "${GALENA_SHADER_SRC_HEADER}"
            "const char *Get${ENTRY_NAME}_${EXT}();\n\n")

        string(CONCAT GALENA_SHADER_SRC_IMPL
            "${GALENA_SHADER_SRC_IMPL}"
            "const char *Get${ENTRY_NAME}_${EXT}()\n"
            "{\n"
            "    return R\"("
            "${SHADER_SRC}"
            "    )\";\n"
            "}\n\n"
        )
    endforeach ()
endmacro ()

process_shaders("vs")
process_shaders("fs")

configure_file("${GALENA_SRC_ROOT}/shaders/GShaders.h.in"
    "${GALENA_SRC_ROOT}/shaders/GShaders.h")

set(GALENA_SHADER_SRC_IMPL_FILE "${GALENA_SRC_ROOT}/shaders/GShaders.cpp")

configure_file("${GALENA_SRC_ROOT}/shaders/GShaders.cpp.in"
    "${GALENA_SHADER_SRC_IMPL_FILE}")

# Setup library

set(GALENA_LIBRARY_SRC)

foreach (ENTRY ${GALENA_SRC})
    LIST(APPEND GALENA_LIBRARY_SRC "${GALENA_SRC_ROOT}/${ENTRY}")
endforeach ()

add_library(${GALENA_LIBRARY_NAME}
    ${GALENA_LIBRARY_SRC}
    ${GALENA_SHADER_SRC_IMPL_FILE})

target_include_directories(${GALENA_LIBRARY_NAME} PUBLIC ${GALENA_INCLUDE_DIR})

foreach (ENTRY ${GALENA_EXTERNAL_PUBLIC_LIBS})
    find_package(${ENTRY} REQUIRED)
    target_link_libraries(${GALENA_LIBRARY_NAME} PUBLIC ${ENTRY})
endforeach ()

foreach (ENTRY ${GALENA_EXTERNAL_PRIVATE_LIBS})
    find_package(${ENTRY} REQUIRED)
    target_link_libraries(${GALENA_LIBRARY_NAME} PRIVATE ${ENTRY})
endforeach ()

if (EMSCRIPTEN)
    target_link_options(${GALENA_LIBRARY_NAME} PUBLIC -sUSE_WEBGL2=1 -sFULL_ES3=1)
endif ()

# Setup samples

file(GLOB GALENA_SAMPLES "samples/*.cpp")

foreach (ENTRY ${GALENA_SAMPLES})
    get_filename_component(SAMPLE_NAME "${ENTRY}" NAME_WE)
    add_executable(${SAMPLE_NAME} ${ENTRY})
    target_link_libraries(${SAMPLE_NAME} PRIVATE ${GALENA_LIBRARY_NAME})

    if (EMSCRIPTEN)
        # For each sample, copy it to the HTML directory
        set(ENTRY_ARTIFACT_BASE "${CMAKE_CURRENT_BINARY_DIR}/${SAMPLE_NAME}")
        set(ENTRY_WASM_FILE "${ENTRY_ARTIFACT_BASE}.wasm")
        set(ENTRY_JS_FILE "${ENTRY_ARTIFACT_BASE}.js")
        add_custom_command(TARGET ${SAMPLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ENTRY_WASM_FILE}" "${GALENA_SAMPLE_WASM_DEPLOY_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ENTRY_JS_FILE}" "${GALENA_SAMPLE_WASM_DEPLOY_DIR}"
        )
    endif ()
endforeach ()
