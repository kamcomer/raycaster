#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct AeRenderer AeRenderer;

typedef enum { AE_RENDERER_BACKEND_SDL } AeRenderBackendType;
typedef enum { AE_RENDERER_TECHNIQUE_RAYCASTER } AeRenderTechniqueType;
typedef enum { AE_RENDERER_SCREEN_MODE_FULLSCREEN } AeRendererScreenMode;

typedef struct AeRendererConfig {
  AeRenderBackendType backend;
  AeRendererScreenMode screen_mode;
  AeRenderTechniqueType technique;
  uint32_t width;
  uint32_t height;
  uint32_t target_fps;
  bool show_fps;
  bool use_gpu;
  bool disable_sprites;
} AeRendererConfig;

#endif // RENDERER_H
