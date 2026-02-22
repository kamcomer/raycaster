#include "scene.h"
#include "render_fp.h"
#include "config.h"
#include "event.h"
#include "timing.h"
#include "renderer.h"

#include <stdio.h>
#include <SDL_image.h>

#define FIXED_DT 0.0166667f  // 60 Hz fixed timestep

int initialize_SDL(void)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n",
            SDL_GetError());
    return 1;
  }

  if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0)
  {
    fprintf(stderr, "IMG_Init Error: %s\n", IMG_GetError());
    SDL_Quit();
    return 1;
  }
  return 0;
}

void release_SDL_resources(void)
{
  IMG_Quit();
  SDL_Quit();
}

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  initialize_SDL();
  Config *config = init_config();
  WindowCtx *window_ctx = create_window_ctx(config->window_config);
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
  release_SDL_resources();
  return 0;
}
