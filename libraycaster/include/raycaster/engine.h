#ifndef ENGINE_H
#define ENGINE_H

#include "raycaster/input.h"
#include <stdbool.h>

typedef struct RcEngine RcEngine;
typedef struct RcEngineConfig {
  const char *title;
  uint32_t width;
  uint32_t height;
  uint32_t target_fps;
  bool show_fps;
  const char *map_file;
  uint32_t strip_count;
  RcInputBackend input_backend;
  bool use_gpu;
} RcEngineConfig;

void rc_engine_config_set_defaults(RcEngineConfig *cfg);

#endif // ENGIN_H
