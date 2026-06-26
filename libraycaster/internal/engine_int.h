#ifndef ENGINE_INT_H
#define ENGINE_INT_H

#include "raycaster/raycaster.h"

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

  uint32_t last_time;
  uint32_t current_time;
  float accumulator;

  uint32_t frame_count;
  float fps;
  uint32_t fps_timer;

  RcTextureData *textures;
};

#endif // ENGINE_INT_H
