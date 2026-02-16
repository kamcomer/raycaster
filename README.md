# Raycaster

A Wolfenstein 3D-style raycasting engine written in C using SDL2.

## Features

- First-person 3D rendering using raycasting
- Textured walls with multiple texture support
- Sprite rendering (static and dynamic)
- 2D debug views (map, player rays, actor visualization)
- Configurable window and rendering settings
- Player movement and collision detection

## Building

### Prerequisites

- CMake 3.19+
- SDL2
- SDL2_image

### Build Commands

```bash
mkdir build && cd build
cmake ..
make
```

On macOS with Homebrew:
```bash
brew install sdl2 sdl2_image
```

On Linux:
```bash
sudo apt install libsdl2-dev libsdl2-image-dev
```

## Running

```bash
./build/main
```

## Configuration

Edit `config.ini` to customize:

| Setting | Description | Default |
|---------|-------------|---------|
| `win_title` | Window title | "Game Window" |
| `win_width` | Window width | 480 |
| `win_height` | Window height | 480 |
| `win_max_fps` | Target FPS | 120 |
| `win_show_fps` | Show FPS counter (0/1) | 1 |

## Controls

- **W/S** - Move forward/backward
- **A/D** - Strafe left/right
- **Left/Right Arrows** - Rotate view

## Project Structure

```
src/
├── main.c              # Entry point
├── core/               # Configuration
├── actors/             # Player and NPC logic
├── sim/                # Scene, map, sprites
├── render/             # Texture handling
├── platform/sdl/      # SDL2 window/renderer
├── event/              # Event handling
├── input/              # Input processing
└── util/               # Utilities

include/                # Header files
assets/
├── maps/               # Level definitions
└── textures/           # Wall textures
```

## Map Format

Maps are text files with:
- First line: width height
- Subsequent lines: wall types (0 = empty, 1+ = textured walls)
- Texture indices map to files in `assets/textures/`
