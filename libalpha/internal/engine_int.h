#ifndef ENGINE_INT_H
#define ENGINE_INT_H

#include "internal/assets/asset_manager_int.h"
#include "internal/camera_int.h"
#include "raycaster/engine.h"
#include "raycaster/level.h"

typedef enum AeEngineResult {
  AE_ENGINE_RES_OK,
  AE_ENGINE_RES_INVD_ARG,
  AE_ENGINE_RES_ALLOC_ERR,
} AeengineResult;

struct AeEngine {
  AeAssetManager asset_manager;
  AeRenderer *renderer;
  AeLevel *level;
  AeCamera *camera;
  AeInput *input;
  void *game_state;
  AeEngineConfig config;

  float delta_time;
  float fps;
  float accumulator;

  uint32_t last_time;
  uint32_t current_time;
  uint32_t frame_count;
  uint32_t fps_timer;

  bool running;
};

#endif // ENGINE_INT_H
