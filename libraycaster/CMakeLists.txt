cmake_minimum_required(VERSION 3.19)

project(libraycaster VERSION 0.1 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(RAYCASTER_SOURCES
    src/util/vector.c
    src/sim/map_world.c
    src/sim/actor.c
    src/sim/camera.c
    src/sim/engine.c
    src/render/frame.c
    src/render/texture.c
    src/render/gpu_renderer.c
    src/input/input_sdl.c
    src/platform/sdl/sdl_window.c
    src/platform/sdl/sdl_renderer.c
    src/platform/sdl/sdl_window_event.c
)

set(RAYCASTER_PUBLIC_HEADERS
    include/raycaster/types.h
    include/raycaster/input.h
    include/raycaster/camera.h
    include/raycaster/world.h
    include/raycaster/actor.h
    include/raycaster/raycaster.h
    include/raycaster/gpu_renderer.h
)

add_library(raycaster STATIC ${RAYCASTER_SOURCES} ${RAYCASTER_PUBLIC_HEADERS})

target_include_directories(raycaster PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(raycaster PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/actors
    ${CMAKE_SOURCE_DIR}/include/core
    ${CMAKE_SOURCE_DIR}/include/interface
    ${CMAKE_SOURCE_DIR}/include/sim
    ${CMAKE_SOURCE_DIR}/include/util
    ${CMAKE_SOURCE_DIR}/include/render
    ${CMAKE_SOURCE_DIR}/include/event
    ${CMAKE_SOURCE_DIR}/include/input
)

target_link_libraries(raycaster PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

set_target_properties(raycaster PROPERTIES
    PUBLIC_HEADER "${RAYCASTER_PUBLIC_HEADERS}"
)
