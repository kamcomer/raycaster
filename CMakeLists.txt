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
  src/sim/render_2d.c
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
)

# Add project executable
add_executable(main ${SOURCES})
target_include_directories(main PRIVATE ${INCLUDES})

set(SDL2IMAGE_VENDORED OFF)
add_subdirectory(packages/SDL EXCLUDE_FROM_ALL)
add_subdirectory(packages/SDL_image EXCLUDE_FROM_ALL)

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

target_link_libraries(main PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2_image::SDL2_image)
target_include_directories(main PRIVATE SDL2::SDL2 SDL2_image::SDL2_image)

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
