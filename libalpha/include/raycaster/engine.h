#ifndef ENGINE_H
#define ENGINE_H

#include "raycaster/input.h"
#include "raycaster/renderer.h"
#include <stdbool.h>

typedef struct AeEngine AeEngine;

typedef struct {
  AeRendererConfig rend_config;
  RcInputBackend input_backend;
} AeEngineConfig;

#endif // ENGIN_H
