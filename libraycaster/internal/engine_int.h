#ifndef ENGINE_INT_H
#define ENGINE_INT_H

#include "raycaster/raycaster.h"
#include <SDL3/SDL.h>

struct RcEngine {
  RcEngineConfig config;
  RcRenderer *renderer;
  RcLevel *level;
  RcCamera *camera;
  RcInput *input;

  void *game_state;
  RcUpdateFn update_fn;
  RcRenderFn render_fn;
  bool running;
  float delta_time;

  SDL_Window *window;
  bool use_gpu;

  uint32_t last_time;
  uint32_t current_time;
  float accumulator;

  uint32_t frame_count;
  float fps;
  uint32_t fps_timer;

  RcTextureData *textures;

  SDL_Texture *wall_texture;
  SDL_Texture *wall_textures[11];
  SDL_Texture *floor_ceil_texture;
  SDL_Texture *sprite_texture;
  SDL_Texture *floor_texture;
  SDL_Texture *ceiling_texture;
  SDL_Texture *sprite_textures[3];
};

#endif // ENGINE_INT_H
