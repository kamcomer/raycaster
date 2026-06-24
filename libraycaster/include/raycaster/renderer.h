#ifndef RENDERER_H
#define RENDERER_H

#include "raycaster/engine.h"
#include <SDL3/SDL.h>

typedef struct RcRenderer RcRenderer;

RcRenderer *rc_renderer_create(SDL_Window *w, bool use_gpu);
void rc_renderer_render(RcEngine *e);
void rc_renderer_destroy(RcRenderer *r);

#endif // RENDERER_H
