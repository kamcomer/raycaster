#include "scene.h"
#include "render_fp.h"
#include "config.h"
#include "event.h"
#include "timing.h"
#include "renderer.h"
#include "raycaster/gpu_renderer.h"

#include <stdio.h>
#include <SDL3_image/SDL_image.h>

#define FIXED_DT 0.0166667f  // 60 Hz fixed timestep

int initialize_SDL(void)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n",
            SDL_GetError());
    return 1;
  }

  return 0;
}

void release_SDL_resources(void)
{
  SDL_Quit();
}

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  if (initialize_SDL() != 0)
  {
    return 1;
  }

  Config *config = init_config();
  if (!config)
  {
    fprintf(stderr, "Failed to initialize config\n");
    release_SDL_resources();
    return 1;
  }

  WindowCtx *window_ctx = create_window_ctx(config->window_config);
  if (!window_ctx)
  {
    fprintf(stderr, "Failed to create window context\n");
    free_config(config);
    release_SDL_resources();
    return 1;
  }

  printf("Creating scene...\n");
  Scene *scene = create_scene(window_ctx, "assets/maps/map.txt");

  GameTiming timing;
  timing_init(&timing);

  int target_fps = config->window_config->max_fps;

  while (!window_ctx->state.quit)
  {
    handle_window_events(window_ctx->window);

    timing_update(&timing);

    while (timing.accumulator >= FIXED_DT)
    {
      update_player(&scene->player, FIXED_DT, scene->map);
      timing.accumulator -= FIXED_DT;
    }

    timing_cap_fps(target_fps);

    render_fp_scene(scene);
    render_present(window_ctx->renderer);
  }

  free_scene(scene);
  free_window_ctx(window_ctx);
  free_config(config);
  release_SDL_resources();
  return 0;
}
