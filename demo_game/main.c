#include "alpha.h"
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define DEFAULT_FPS_TARGET 120

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  AEngineConfig config;
  ARendererConfig rend_config = {
      .backend = A_RENDERER_BACKEND_SDL,
      .screen_mode = A_RENDERER_SCREEN_MODE_FULLSCREEN,
      .technique = A_RENDERER_TECHNIQUE_RAYCASTER,
      .width = DEFAULT_WIDTH,
      .height = DEFAULT_HEIGHT,
      .target_fps = DEFAULT_FPS_TARGET,
      .show_fps = true,
      .use_gpu = false,
      .disable_sprites = true,
  };

  config.rend_config = rend_config;
  config.input_backend = A_INPUT_BACKEND_SDL;

  AEngine *engine;
  AResult res = a_engine_create(config, &engine);
  if (res != A_RES_OK) {
    fprintf(stderr, "Failed to create engine\n");
    return 1;
  }

  ALevel *level = a_level_load_from_file("assets/maps/map.txt");
  if (!level) {
    fprintf(stderr, "Failed to load level\n");
    a_engine_destroy(engine);
    return 1;
  }

  a_engine_load_level(engine, level);

  printf("Starting game loop...\n");
  printf("Controls: W/S/A/D to move, Left/Right arrows to rotate, ESC to quit\n");

  a_engine_run(engine);

  a_engine_destroy(engine);

  return 0;
}
