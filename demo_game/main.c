#include <raycaster/raycaster.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  RcEngineConfig config;
  rc_engine_config_set_defaults(&config);
  config.title = "Raycaster Game";
  config.width = 800;
  config.height = 600;
  config.target_fps = 120;
  config.show_fps = true;
  config.strip_count = 1;
  config.input_backend = RC_INPUT_BACKEND_SDL;
  config.use_gpu = true;

  RcEngine *engine = rc_engine_create(config);
  if (!engine) {
    fprintf(stderr, "Failed to create engine\n");
    return 1;
  }

  RcLevel *world = rc_level_load_from_file("assets/maps/map.txt");
  if (!world) {
    fprintf(stderr, "Failed to load level\n");
    rc_engine_destroy(engine);
    return 1;
  }

  rc_engine_load_level(engine, world);

  printf("Starting game loop...\n");
  printf("Controls: W/S/A/D to move, Left/Right arrows to rotate, ESC to quit\n");

  rc_engine_run(engine);

  rc_engine_destroy(engine);

  return 0;
}
