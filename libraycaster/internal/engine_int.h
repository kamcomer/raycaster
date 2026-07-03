#ifndef ENGINE_INT_H
#define ENGINE_INT_H

#include "internal/resource/texture_int.h"
#include "raycaster/engine.h"
#include "raycaster/raycaster.h"

typedef enum AeEngineResult {
  AE_ENGINE_RES_OK,
  AE_ENGINE_RES_INVD_ARG,
  AE_ENGINE_RES_ALLOC_ERR,
} AeengineResult;

struct AeEngine {
  AeRenderer *renderer;
  AeLevel *level;
  AeCamera *camera;
  AeInput *input;
  void *game_state;
  AeTextureManager texture_manager;
  AeEngineConfig config;

  bool running;
  float delta_time;

  uint32_t last_time;
  uint32_t current_time;
  float accumulator;

  uint32_t frame_count;
  float fps;
  uint32_t fps_timer;
};

#endif // ENGINE_INT_H
