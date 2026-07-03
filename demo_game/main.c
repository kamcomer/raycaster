#include "raycaster/raycaster.h"
#include "raycaster/renderer.h"
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define DEFAULT_FPS_TARGET 120

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  RcEngineConfig config;
  RcRendererConfig rend_config = {
      .backend = RC_RENDERER_BACKEND_SDL,
      .screen_mode = RC_RENDERER_SCREEN_MODE_FULLSCREEN,
      .title = "Raycaster",
      .width = DEFAULT_WIDTH,
      .height = DEFAULT_HEIGHT,
      .target_fps = DEFAULT_FPS_TARGET,
      .show_fps = true,
      .use_gpu = false,
      .disable_sprites = true,
  };

  rc_engine_config_set_defaults(&config);
  config.rend_config = rend_config;
  config.strip_count = 1;
  config.input_backend = RC_INPUT_BACKEND_SDL;

  RcEngine *engine = rc_engine_create(config);
  if (!engine) {
    fprintf(stderr, "Failed to create engine\n");
    return 1;
  }

  const float cam_pos = 3.5;

  RcCamera *cam = rc_camera_create(config.rend_config.width, config.rend_config.height);
  rc_camera_set_position(cam, cam_pos, cam_pos);
  rc_camera_set_direction(cam, -1.0, 0.0);
  rc_engine_set_camera(engine, cam);

  RcLevel *level = rc_level_load_from_file("assets/maps/map.txt");
  if (!level) {
    fprintf(stderr, "Failed to load level\n");
    rc_engine_destroy(engine);
    return 1;
  }

  rc_engine_load_level(engine, level);

  printf("Starting game loop...\n");
  printf("Controls: W/S/A/D to move, Left/Right arrows to rotate, ESC to quit\n");

  rc_engine_run(engine);

  rc_engine_destroy(engine);

  return 0;
}
