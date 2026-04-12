cmake_minimum_required(VERSION 3.10.0)

project(Galena)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)

option(GALENA_BUILD_SAMPLES "Whether Galena should build samples" ON)

set(GALENA_SRC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(GALENA_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(GALENA_LIBRARY_NAME "${PROJECT_NAME}")
set(GALENA_SAMPLE_WASM_DEPLOY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/html/js")

set(GALENA_SRC
    GEngine.cpp
    GEngineData.cpp
    GEngineResources.cpp
    GFramebuffer.cpp
    GHwBuffer.cpp
    GImage.cpp
    GPostprocessRenderer.cpp
    GTexture.cpp
    GShader.cpp
    GVersion.cpp)

set(GALENA_EXTERNAL_DEPS)
set(GALENA_MAKEAVAILABLE_LIST)

macro (declare_external_deps DEP_NAME DEP_REPO DEP_TAG DEP_LINK)
    FetchContent_Declare("${DEP_NAME}"
        GIT_REPOSITORY "${DEP_REPO}"
        GIT_TAG "${DEP_TAG}"
        SOURCE_DIR "${GALENA_SRC_ROOT}/external/${DEP_NAME}")

    list(APPEND GALENA_MAKEAVAILABLE_LIST "${DEP_NAME}")
    list(APPEND GALENA_EXTERNAL_DEPS "${DEP_LINK}")
endmacro ()

declare_external_deps(glm
    "https://github.com/g-truc/glm.git"
    e8642318a099b9b6ff432bed8b8b90b4226a9edd
    glm::glm)

# Configure version

find_package(Git)

set(GALENA_VERSION 0)

if (${Git_FOUND})
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)

    string(SUBSTRING "${GIT_COMMIT_HASH}" 0 8 GALENA_VERSION)
else ()
    message(WARNING "Git executable not found: setting version to 0")
endif ()

configure_file(
    "${GALENA_SRC_ROOT}/GVersion.cpp.in"
    "${GALENA_SRC_ROOT}/GVersion.cpp")

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

configure_file(
    "${GALENA_SRC_ROOT}/shaders/GShaderData.h.in"
    "${GALENA_SRC_ROOT}/shaders/GShaderData.h")

set(GALENA_SHADER_SRC_IMPL_FILE "${GALENA_SRC_ROOT}/shaders/GShaderData.cpp")

configure_file(
    "${GALENA_SRC_ROOT}/shaders/GShaderData.cpp.in"
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

if (EMSCRIPTEN)
    target_link_options(${GALENA_LIBRARY_NAME} PUBLIC -sUSE_WEBGL2=1 -sFULL_ES3=1)
endif ()

FetchContent_MakeAvailable(${GALENA_MAKEAVAILABLE_LIST})

foreach (ENTRY ${GALENA_EXTERNAL_DEPS})
    target_link_libraries(${GALENA_LIBRARY_NAME} PRIVATE ${ENTRY})
endforeach ()

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
        set(ENTRY_DATA_FILE "${ENTRY_ARTIFACT_BASE}.data")
        add_custom_command(TARGET ${SAMPLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ENTRY_WASM_FILE}" "${GALENA_SAMPLE_WASM_DEPLOY_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ENTRY_JS_FILE}" "${GALENA_SAMPLE_WASM_DEPLOY_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ENTRY_DATA_FILE}" "${GALENA_SAMPLE_WASM_DEPLOY_DIR}"
        )
        target_link_options(${SAMPLE_NAME} PRIVATE
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/html/assets/monde.png@/assets/monde.png)
    endif ()
endforeach ()
