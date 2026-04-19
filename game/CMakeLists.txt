set(GAME_SRC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(GAME_EXECUTABLE_NAME "Lounge")

set(GAME_SRC
    main.cpp
    GGameInstance.cpp)

# Setup game sources

set(GAME_EXECUTABLE_SRC)

foreach (ENTRY ${GAME_SRC})
    list(APPEND GAME_EXECUTABLE_SRC "${GAME_SRC_ROOT}/${ENTRY}")
endforeach ()

add_executable(${GAME_EXECUTABLE_NAME}
    ${GAME_EXECUTABLE_SRC})

target_link_libraries(${GAME_EXECUTABLE_NAME} PRIVATE
    ${GALENA_LIBRARY_NAME})

set(GAME_ARTIFACT_BASE "${CMAKE_CURRENT_BINARY_DIR}/${GAME_EXECUTABLE_NAME}")

if (EMSCRIPTEN)
    set(GAME_WASM_DEPLOY_DIR "${CMAKE_CURRENT_LIST_DIR}/../html/js/wasm")
    
    target_link_options(${GAME_EXECUTABLE_NAME} PRIVATE
        --preload-file ${CMAKE_CURRENT_LIST_DIR}/../html/assets/monde.png@/assets/monde.png)
    
    set(GAME_WASM_FILE "${GAME_ARTIFACT_BASE}.wasm")
    set(GAME_JS_FILE "${GAME_ARTIFACT_BASE}.js")
    set(GAME_DATA_FILE "${GAME_ARTIFACT_BASE}.data")

    add_custom_command(TARGET ${GAME_EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${GAME_WASM_FILE}" "${GAME_WASM_DEPLOY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${GAME_JS_FILE}" "${GAME_WASM_DEPLOY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${GAME_DATA_FILE}" "${GAME_WASM_DEPLOY_DIR}")
endif ()
