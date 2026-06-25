cmake_minimum_required(VERSION 3.19)

project(game VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

include_directories(
    ${CMAKE_SOURCE_DIR}/libraycaster/include
)

add_executable(game main.c)

target_link_libraries(game PRIVATE raycaster)

add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:game>/assets"
    COMMENT "Copying assets to build directory..."
)
