#ifndef SDL_RENDERER_INT_H
#define SDL_RENDERER_INT_H

#include "internal/i_alpha.h"
#include <SDL3/SDL.h>

typedef struct {
  ARendererConfig config;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *screen;
  double *z_buffer;
  uint32 *framebuffer;
  ADimensions *window_dimensions;
} SdlRendererBackendData;

AResult a_sdl_renderer_create(ARendererConfig config, ARendererBackend **out);
AResult a_sdl_renderer_init(ARendererConfig config, ARendererBackend *out);
AResult a_sdl_renderer_destroy(ARendererBackend *backend);
AResult a_sdl_renderer_deinit(ARendererBackend *backend);

extern ARendererBackendVtbl renderer_backend_vtbl;

#endif // SDL_RENDERER_INT_H
