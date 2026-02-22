#ifndef CONFIG_H
#define CONFIG_H

#include "window.h"
#include "player.h"

// // Env defines
// #define REND_2D true
// #define REND_FP true
// #define DEBUG
// #define TEXTURES_PNG true

#define CONFIG_PATH "config.ini"
#define DEFAULT_WIN_TITLE "Game Window"
#define DEFAULT_WIN_WIDTH (DEFAULT_MAP_UNIT_SIZE * DEFAULT_MAP_WIDTH)
#define DEFAULT_WIN_HEIGHT (DEFAULT_MAP_UNIT_SIZE * DEFAULT_MAP_HEIGHT)
#define DEFAULT_WIN_MAX_FPS 120

typedef struct
{
    WindowConfig *window_config;
    PlayerConfig *player_config;
} Config;

Config *init_config(void);
void print_config(void);

#endif