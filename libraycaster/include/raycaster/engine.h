#ifndef ENGINE_H
#define ENGINE_H

#include "raycaster/input.h"
#include "raycaster/renderer.h"
#include <stdbool.h>

typedef struct RcEngine RcEngine;

typedef struct {
  RcRendererConfig rend_config;
  const char *map_file;
  uint32_t strip_count;
  RcInputBackend input_backend;
} RcEngineConfig;

void rc_engine_config_set_defaults(RcEngineConfig *cfg);

#endif // ENGIN_H
