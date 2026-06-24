
#include "raycaster/renderer.h"
#include "internal/engine_int.h"
#include "internal/renderer/cpu_renderer_int.h"
#include <stdlib.h>

RcRenderer *rc_renderer_create(SDL_Window *w, bool use_gpu)
{
  if (use_gpu) {
    return NULL;
  } else {
    return cpu_renderer_create(w);
  }
}

void rc_renderer_render(RcEngine *e) { e->renderer->vtbl->render(e); }
void rc_renderer_destroy(RcRenderer *r)
{
  if (!r)
    return;
  CPURendererData *data = (CPURendererData *)r->impl;
  if (data->renderer)
    SDL_DestroyRenderer(data->renderer);
  free(data);
  free(r);
}
void *rc_renderer_get_renderer(RcRenderer *r) { return r->vtbl->get_renderer(r); }
