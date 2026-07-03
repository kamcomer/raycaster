#ifndef RENDERER_INT_H
#define RENDERER_INT_H

#include "raycaster/engine.h"
#include "raycaster/renderer.h"

typedef struct RcRendererVtbl RcRendererVtbl;

struct RcRenderer {
  const RcRendererVtbl *vtbl;
  void *impl;
};

struct RcRendererVtbl {
  void (*render)(RcEngine *engine);
  void (*destroy)(AeRenderer *renderer);
  void *(*get_renderer)(AeRenderer *renderer);
  void *(*get_window)(AeRenderer *renderer);
};

void *rc_renderer_get_renderer(AeRenderer *renderer);
void *rc_renderer_get_window(AeRenderer *renderer);
void rc_render_software_frame(RcEngine *eengine, uint32_t *framebuffer, double *z_buffer);

#endif // RENDERER_INT_H
