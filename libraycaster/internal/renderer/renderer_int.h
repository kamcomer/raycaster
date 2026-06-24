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
  void *(*get_renderer)(RcRenderer *r);
};

void *rc_renderer_get_renderer(RcRenderer *r);

#endif // RENDERER_INT_H
