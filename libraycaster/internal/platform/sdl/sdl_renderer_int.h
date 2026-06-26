#ifndef SDL_RENDERER_INT_H
#define SDL_RENDERER_INT_H

#include "internal/renderer_int.h"
#include <SDL3/SDL.h>

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *screen;
  uint32_t *framebuffer;
  double *z_buffer;
  uint32_t width;
  uint32_t height;
} SdlRendererSoftwareBackendData;

RcRenderer *sdl_renderer_create(RcRendererConfig config);

extern RcRendererVtbl renderer_vtbl;

#endif // SDL_RENDERER_INT_H
