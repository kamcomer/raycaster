include(FetchContent)

cmake_minimum_required(VERSION 3.19)

project(libalpha VERSION 0.1 LANGUAGES C)

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
# FetchContent_Declare(
#     SDL3_shadercross
#     GIT_REPOSITORY https://github.com/libsdl-org/SDL_shadercross.git
#     GIT_TAG main
#     GIT_SHALLOW TRUE
#     GIT_SUBMODULES "external/SPIRV-Cross;external/SPIRV-Headers;external/SPIRV-Tools;external/DirectXShaderCompiler"
#   )

# set(SDLSHADERCROSS_VENDORED ON)
FetchContent_MakeAvailable(SDL3)
FetchContent_MakeAvailable(SDL3_image)
# FetchContent_MakeAvailable(SDL3_shadercross)

set(RAYCASTER_SOURCES
    src/level/level.c
    src/level/level_raycaster.c
    src/input.c
    src/actor.c
    src/camera.c
    src/engine.c
    src/renderer/renderer.c
    src/renderer/technique_raycaster.c
    src/assets/texture.c
    src/platform/sdl/sdl_input.c
    src/platform/sdl/sdl_renderer.c
    kutils/str.c
)

# set(RAYCASTER_PUBLIC_HEADERS
#     include/raycaster/types.h
#     include/raycaster/input.h
#     include/raycaster/camera.h
#     include/raycaster/level.h
#     include/raycaster/actor.h
#     include/raycaster/raycaster.h
#     include/raycaster/renderer.h
#     include/raycaster/engine.h
#   )


add_library(alpha STATIC ${RAYCASTER_SOURCES} include/alpha.h)

target_include_directories(alpha PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(alpha PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINAR}
  )


# target_link_libraries(alpha PUBLIC SDL3::SDL3 SDL3_image::SDL3_image SDL3_shadercross::SDL3_shadercross)
target_link_libraries(alpha PUBLIC SDL3::SDL3 SDL3_image::SDL3_image)

set_target_properties(alpha PROPERTIES
    PUBLIC_HEADER include/alpha.h
)
