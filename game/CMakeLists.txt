cmake_minimum_required(VERSION 3.19)

project(game VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

include_directories(
    ${CMAKE_SOURCE_DIR}/libraycaster/include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/packages/SDL/include
    ${CMAKE_SOURCE_DIR}/packages/SDL_image/include
)

add_executable(game main.c)

target_link_libraries(game PRIVATE raycaster)
