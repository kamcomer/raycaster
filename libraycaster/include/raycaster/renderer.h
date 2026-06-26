#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>
#include <stdint.h>

struct RcEngine;
typedef struct RcRenderer RcRenderer;

typedef enum { RC_RENDERER_BACKEND_SDL } RcRendererBackend;
typedef enum { RC_RENDERER_SCREEN_MODE_FULLSCREEN } RcRendererScreenMode;

typedef struct RcRendererConfig {
  const char *title;
  uint32_t width;
  uint32_t height;
  uint32_t target_fps;
  bool show_fps;
  bool use_gpu;
  RcRendererBackend backend;
  RcRendererScreenMode screen_mode;

} RcRendererConfig;

RcRenderer *rc_renderer_create(RcRendererConfig config);
void rc_renderer_render(struct RcEngine *e);
void rc_renderer_destroy(RcRenderer *r);

#endif // RENDERER_H
