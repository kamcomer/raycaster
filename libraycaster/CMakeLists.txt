include(FetchContent)

cmake_minimum_required(VERSION 3.19)

project(libraycaster VERSION 0.1 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.10
    GIT_SHALLOW TRUE
)
FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-3.4.4
    GIT_SHALLOW TRUE
)
FetchContent_Declare(
    SDL3_shadercross
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_shadercross.git
    GIT_TAG main
    GIT_SHALLOW TRUE
    GIT_SUBMODULES "external/SPIRV-Cross;external/SPIRV-Headers;external/SPIRV-Tools;external/DirectXShaderCompiler"
)

set(SDLSHADERCROSS_VENDORED ON)
FetchContent_MakeAvailable(SDL3)
FetchContent_MakeAvailable(SDL3_image)
FetchContent_MakeAvailable(SDL3_shadercross)

# Embed HLSL shaders as C strings
set(SHADER_EMBED_SRC "${CMAKE_CURRENT_BINARY_DIR}/shaders_embed.c")

add_custom_command(
    OUTPUT ${SHADER_EMBED_SRC}
    ${CMAKE_CURRENT_BINARY_DIR}/shaders_embed.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/wall.hlsl
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/floor.hlsl
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/sprite.hlsl
    COMMAND ${CMAKE_COMMAND}
    -DSHADER_DIR=${CMAKE_CURRENT_SOURCE_DIR}/shaders
    -DOUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/embed_shaders.cmake
    VERBATIM
)

set(RAYCASTER_SOURCES
    src/util/vector.c
    src/util/general.c
    src/level/level.c
    src/level/map_level.c
    src/input/input.c
    src/sim/actor.c
    src/sim/camera.c
    src/sim/engine.c
    src/renderer/renderer.c
    src/renderer/gpu_renderer.c
    src/renderer/cpu_renderer.c
    src/platform/sdl/sdl_input.c
    src/platform/sdl/sdl_window.c
    src/platform/sdl/sdl_renderer.c
    src/platform/sdl/sdl_window_event.c
    ${SHADER_EMBED_SRC}
)

set(RAYCASTER_PUBLIC_HEADERS
    include/raycaster/types.h
    include/raycaster/input.h
    include/raycaster/camera.h
    include/raycaster/level.h
    include/raycaster/actor.h
    include/raycaster/raycaster.h
    include/raycaster/renderer.h
    include/raycaster/engine.h
    include/raycaster/vector.h
)

add_library(raycaster STATIC ${RAYCASTER_SOURCES} ${RAYCASTER_PUBLIC_HEADERS})

target_include_directories(raycaster PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(raycaster PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/packages/SDL_shadercross/include
)

target_link_libraries(raycaster PUBLIC SDL3::SDL3 SDL3_image::SDL3_image SDL3_shadercross::SDL3_shadercross)

set_target_properties(raycaster PROPERTIES
    PUBLIC_HEADER "${RAYCASTER_PUBLIC_HEADERS}"
)

# Unit tests
option(RAYCASTER_BUILD_TESTS "Build unit tests" OFF)

if(RAYCASTER_BUILD_TESTS)
    add_executable(test_map_level
        tests/unity.c
        tests/test_map_level.c
        src/level/level.c
        src/level/map_level.c
        src/util/general.c
    )
    target_include_directories(test_map_level PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )
    add_test(NAME map_level COMMAND test_map_level)
endif()
