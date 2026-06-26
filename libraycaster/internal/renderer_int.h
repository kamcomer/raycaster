#ifndef RENDERER_INT_H
#define RENDERER_INT_H

#include "internal/engine_int.h"
#include "raycaster/renderer.h"

typedef struct RcRendererVtbl RcRendererVtbl;

struct RcRenderer {
  const RcRendererVtbl *vtbl;
  void *impl;
};

struct RcRendererVtbl {
  void (*render)(RcEngine *e);
  void (*destroy)(RcRenderer *r);
  void *(*get_renderer)(RcRenderer *r);
  void *(*get_window)(RcRenderer *r);
};

void *rc_renderer_get_renderer(RcRenderer *r);
void *rc_renderer_get_window(RcRenderer *r);
void rc_render_software_frame(RcEngine *e, uint32_t *framebuffer, double *z_buffer);

#endif // RENDERER_INT_H
