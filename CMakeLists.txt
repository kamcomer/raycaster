# Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.19)

# Set the project name and version
project(raycaster VERSION 0.1 LANGUAGES C)

# Generate a compile commands file for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable compiler warnings
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add the raycaster library
add_subdirectory(libraycaster)

# Add SDL (needed by both main and game)
set(SDL3IMAGE_VENDORED OFF)
add_subdirectory(packages/SDL EXCLUDE_FROM_ALL)
add_subdirectory(packages/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(packages/SDL_shadercross EXCLUDE_FROM_ALL)

# Add shader directory
set(SHADERS_DIR "${CMAKE_SOURCE_DIR}/shaders")

# Add game
add_subdirectory(game)

# Define source files explicitly
set(SOURCES
  src/main.c
  src/core/Config.c
  src/event/event.c
  src/interface/window_ctx.c
  src/platform/sdl/sdl_input.c
  src/platform/sdl/sdl_renderer.c
  src/platform/sdl/sdl_window.c
  src/platform/sdl/sdl_window_event.c
  src/render/texture.c
  src/sim/Scene.c
  src/sim/render_fp.c
  src/sim/map.c
  src/sim/sprite.c
  src/actors/Player.c
  src/actors/actor.c
  src/util/utility.c
  src/util/vector.c
  src/util/timing.c
  src/render/hud.c
)

# Include directories
set(INCLUDES
  ${CMAKE_SOURCE_DIR}/include/actors
  ${CMAKE_SOURCE_DIR}/include/core
  ${CMAKE_SOURCE_DIR}/include/interface
  ${CMAKE_SOURCE_DIR}/include/sim
  ${CMAKE_SOURCE_DIR}/include/util
  ${CMAKE_SOURCE_DIR}/include/render
  ${CMAKE_SOURCE_DIR}/include/event
  ${CMAKE_SOURCE_DIR}/include/input
  ${CMAKE_SOURCE_DIR}/shaders
  ${CMAKE_SOURCE_DIR}/libraycaster/include
)

# Add project executable
add_executable(main ${SOURCES})
target_include_directories(main PRIVATE ${INCLUDES})

if(WIN32)
  add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${SDL2_DLL} $<TARGET_FILE_DIR:main>
  )

  add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${SDL2_IMAGE_DLL} $<TARGET_FILE_DIR:main>
  )
endif()

target_link_libraries(main PRIVATE SDL3::SDL3 SDL3_image::SDL3_image raycaster)
target_include_directories(main PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

# Custom targets
add_custom_target(distclean
  COMMAND rm -rf ${CMAKE_BINARY_DIR}
  COMMENT "Remove build directory and all generated files"
)

# Unit tests
enable_testing()

set(TEST_INCLUDES
  ${CMAKE_SOURCE_DIR}/include/actors
  ${CMAKE_SOURCE_DIR}/include/core
  ${CMAKE_SOURCE_DIR}/include/interface
  ${CMAKE_SOURCE_DIR}/include/sim
  ${CMAKE_SOURCE_DIR}/include/util
  ${CMAKE_SOURCE_DIR}/include/render
  ${CMAKE_SOURCE_DIR}/include/event
  ${CMAKE_SOURCE_DIR}/include/input
)

add_executable(test_vector
  tests/unity.c
  tests/test_vector.c
  src/util/vector.c
)
target_include_directories(test_vector PRIVATE ${TEST_INCLUDES})
add_test(NAME vector_tests COMMAND test_vector)

add_executable(test_map
  tests/unity.c
  tests/test_map.c
  src/sim/map.c
)
target_include_directories(test_map PRIVATE ${TEST_INCLUDES})
add_test(NAME map_tests COMMAND test_map)

add_executable(test_sprite
  tests/unity.c
  tests/test_sprite.c
  src/sim/sprite.c
)
target_include_directories(test_sprite PRIVATE ${TEST_INCLUDES})
add_test(NAME sprite_tests COMMAND test_sprite)
