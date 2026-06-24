#ifndef CPU_RENDERER_INT_H
#define CPU_RENDERER_INT_H

#include "internal/renderer/renderer_int.h"
#include <SDL3/SDL.h>

typedef struct {
  SDL_Renderer *renderer;
} CPURendererData;

RcRenderer *cpu_renderer_create(SDL_Window *window);

extern RcRendererVtbl cpu_renderer_vtbl;

#endif // CPU_RENDERER_INT_H
