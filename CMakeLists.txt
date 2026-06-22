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

# Place game binary in the source tree alongside assets
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/demo_game)

# Add game
add_subdirectory(demo_game)

# Custom targets
add_custom_target(distclean
  COMMAND rm -rf ${CMAKE_BINARY_DIR}
  COMMENT "Remove build directory and all generated files"
)