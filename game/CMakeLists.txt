set(GAME_SRC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(GAME_EXECUTABLE_NAME "Lounge")

set(GAME_SRC
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
