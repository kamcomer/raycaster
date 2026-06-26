
#include "raycaster/renderer.h"
#include "internal/engine_int.h"
#include "internal/platform/sdl/sdl_renderer_int.h"
#include "internal/renderer_int.h"
#include <stdlib.h>

RcRenderer *rc_renderer_create(RcRendererConfig config)
{
  switch (config.backend) {
  case RC_RENDERER_BACKEND_SDL:
    return sdl_renderer_create(config);
    break;
  }
}

void rc_renderer_render(RcEngine *e) { e->renderer->vtbl->render(e); }
void rc_renderer_destroy(RcRenderer *r)
{
  if (!r)
    return;
  r->vtbl->destroy(r);
  free(r);
}
void *rc_renderer_get_renderer(RcRenderer *r) { return r->vtbl->get_renderer(r); }
void *rc_renderer_get_window(RcRenderer *r) { return r->vtbl->get_window(r); }
