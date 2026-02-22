# AGENTS.md

This file defines how AI agents must operate within this repository.

---

## Project Overview

- **Language**: C
- **Build System**: CMake 3.19+
- **Dependencies**: SDL2, SDL2_image
- **Platform**: Cross-platform (macOS, Linux, Windows)

---

## Build Commands

### Full Build

```bash
mkdir -p build && cd build
cmake ..
make
```

### Incremental Build

```bash
cd build
make
```

### Clean Build

```bash
rm -rf build && mkdir build && cd build
cmake ..
make
```

### Run the Application

```bash
./build/main
```

### Debug Build (VS Code)

The project includes `.vscode/launch.json` for debugging. Press F5 in VS Code to build and debug.

---

## Testing

**No test framework is currently configured for this project.**

If adding tests, use Ceedling, Unity, or similar C testing framework. Avoid introducing new test frameworks without justification.

---

## Linting & Code Quality

### clangd

The project generates `compile_commands.json` for clangd support. Use clangd for:
- Inline diagnostics
- Go-to-definition
- Find references
- Code completion

### Manual Linting

Run clang-tidy if installed:

```bash
clang-tidy src/*.c src/**/*.c -- -Iinclude -Iinclude/actors -Iinclude/core -Iinclude/event -Iinclude/input -Iinclude/interface -Iinclude/render -Iinclude/sim -Iinclude/util
```

---

## Code Style Guidelines

### Formatting

- **Indentation**: 2 spaces (no tabs)
- **Line Length**: Soft limit 100 characters
- **Braces**: K&R style (opening brace on same line)
- **Trailing Whitespace**: Remove

### Header Guards

Use traditional header guards:

```c
#ifndef CONFIG_H
#define CONFIG_H

// ... content ...

#endif // CONFIG_H
```

### Naming Conventions

- **Types (typedef structs)**: PascalCase (e.g., `Vector`, `WindowConfig`)
- **Functions**: snake_case (e.g., `init_config`, `calculate_vector_mag`)
- **Variables**: snake_case (e.g., `window_ctx`, `max_fps`)
- **Constants/Macros**: UPPER_SNAKE_CASE (e.g., `DEFAULT_WIN_WIDTH`, `PI`)
- **Files**: snake_case (e.g., `config.c`, `vector.h`)

### Struct Definitions

Place opening brace on same line as typedef:

```c
typedef struct
{
    int x;
    int y;
} Point;
```

### Function Declarations

- Prefer explicit return types (not relying on K&R implicit int)
- Document complex functions with Doxygen-style comments
- Keep functions focused and small

### Error Handling

- Return `0` for success, non-zero (`-1` or `1`) for failure
- Print errors to `stderr`, not `stdout`
- Check return values of allocation functions (`malloc`, `fopen`, etc.)
- Clean up resources on failure paths
- Use `NULL` for pointer failures

Example:

```c
FILE *file = fopen(CONFIG_PATH, "r");
if (!file)
{
    fprintf(stderr, "Could not open file: %s\n", CONFIG_PATH);
    return NULL;
}
```

### Memory Management

- Always free what you allocate
- Match `malloc` with `free`, `fopen` with `fclose`
- Check for NULL after allocation before use
- Consider using helper allocation macros if needed

### Include Order

1. Associated header (if in .c file)
2. Local headers (quoted)
3. System headers (angle brackets)

```c
#include "config.h"
#include "window_ctx.h"
#include "scene.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
```

### Preprocessor Defines

- Avoid function-like macros when possible (use static inline functions)
- Use `#define` for constants and configuration values
- Comment complex or magic-number macros

### Comments

- Explain *why*, not *what*
- Avoid redundant comments stating the obvious
- Use // for single-line and block comments
- Document public API functions

---

## Project Structure

```
src/
├── main.c              # Entry point
├── core/               # Configuration
├── actors/             # Player and NPC logic
├── sim/                # Scene, map, sprites
├── render/             # Texture handling
├── platform/sdl/       # SDL2 window/renderer
├── event/              # Event handling
├── input/              # Input processing
└── util/               # Utilities

include/                # Header files (mirrors src structure)
assets/
├── maps/               # Level definitions
└── textures/           # Wall textures
```

---

## Configuration

Edit `config.ini` to customize runtime settings:

| Setting       | Description              | Default |
|---------------|--------------------------|---------|
| `win_title`   | Window title            | "Game Window" |
| `win_width`   | Window width            | 480 |
| `win_height`  | Window height           | 480 |
| `win_max_fps` | Target FPS              | 120 |
| `win_show_fps`| Show FPS counter (0/1) | 1 |

---

## Git & Commit Discipline

- Make minimal, focused commits
- Follow conventional commit style: `feat:`, `fix:`, `refactor:`, `test:`, `docs:`, `chore:`
- Never commit secrets or keys (check .gitignore)
- Keep diffs clean: avoid reformatting unrelated code

---

## Platform-Specific Notes

### macOS

Install dependencies via Homebrew:
```bash
brew install sdl2 sdl2_image
```

### Linux

```bash
sudo apt install libsdl2-dev libsdl2-image-dev
```

### Windows

CMake automatically downloads SDL2 from `Packages/` subdirectory.

---

## Dependencies

- **Do not introduce new dependencies** without strong justification
- Prefer standard library over external libraries
- If adding a dependency: explain why, document tradeoffs
