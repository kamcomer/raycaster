#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>
#include <stdint.h>

struct AeEngine;
typedef struct RcRenderer AeRenderer;

typedef enum { RC_RENDERER_BACKEND_SDL } RcRendererBackend;
typedef enum { RC_RENDERER_SCREEN_MODE_FULLSCREEN } RcRendererScreenMode;

typedef struct RcRendererConfig {
  const char *title;
  uint32_t width;
  uint32_t height;
  uint32_t target_fps;
  bool show_fps;
  bool use_gpu;
  bool disable_sprites;
  RcRendererBackend backend;
  RcRendererScreenMode screen_mode;

} RcRendererConfig;

AeRenderer *rc_renderer_create(RcRendererConfig config);
void rc_renderer_render(struct AeEngine *engine);
void ae_renderer_destroy(AeRenderer *renderer);

#endif // RENDERER_H
