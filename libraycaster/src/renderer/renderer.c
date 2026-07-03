
#include "raycaster/renderer.h"
#include "internal/engine_int.h"
#include "internal/platform/sdl/sdl_renderer_int.h"
#include "internal/renderer_int.h"
#include <stdlib.h>

AeRenderer *rc_renderer_create(RcRendererConfig config)
{
  switch (config.backend) {
  case RC_RENDERER_BACKEND_SDL:
    return sdl_renderer_create(config);
    break;
  }
}

void rc_renderer_render(RcEngine *e) { e->renderer->vtbl->render(e); }
void ae_renderer_destroy(AeRenderer *r)
{
  if (!r)
    return;
  r->vtbl->destroy(r);
  free(r);
}
void *rc_renderer_get_renderer(AeRenderer *r) { return r->vtbl->get_renderer(r); }
void *rc_renderer_get_window(AeRenderer *r) { return r->vtbl->get_window(r); }
