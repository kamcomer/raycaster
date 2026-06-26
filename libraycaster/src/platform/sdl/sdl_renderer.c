#include "internal/platform/sdl/sdl_renderer_int.h"
#include <stdlib.h>

static void destroy_impl(SdlRendererSoftwareBackendData *data)
{
  SDL_DestroyTexture(data->screen);
  SDL_DestroyRenderer(data->renderer);
  SDL_DestroyWindow(data->window);
  free(data->framebuffer);
  free(data->z_buffer);
  SDL_Quit();
  free(data);
}

static void destroy(RcRenderer *r) { destroy_impl(r->impl); }

static void *get_renderer(RcRenderer *r)
{
  SdlRendererSoftwareBackendData *data = r->impl;
  return data->renderer;
}

static void *get_window(RcRenderer *r)
{
  SdlRendererSoftwareBackendData *data = r->impl;
  return data->window;
}

static void render(RcEngine *e)
{
  SdlRendererSoftwareBackendData *data = e->renderer->impl;
  rc_render_software_frame(e, data->framebuffer, data->z_buffer);
  SDL_UpdateTexture(data->screen, NULL, data->framebuffer, data->width * 4);
  SDL_RenderClear(data->renderer);
  SDL_RenderTexture(data->renderer, data->screen, NULL, NULL);
  SDL_RenderPresent(data->renderer);
}

static RcRenderer *create_software_renderer(RcRendererConfig config)
{
  SdlRendererSoftwareBackendData *data = calloc(1, sizeof(SdlRendererSoftwareBackendData));
  data->window =
      SDL_CreateWindow(config.title ? config.title : "Raycaster", config.width, config.height, 0);
  if (!data->window) {
    fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
    destroy_impl(data);
    return NULL;
  }

  data->renderer = SDL_CreateRenderer(data->window, NULL);
  if (!data->renderer) {
    destroy_impl(data);
    return NULL;
  }

  RcRenderer *r = malloc(sizeof(RcRenderer));
  if (!r) {
    destroy_impl(data);
    return NULL;
  }

  data->framebuffer = malloc(config.width * config.height * sizeof(uint32_t));
  if (!data->framebuffer) {
    destroy_impl(data);
    return NULL;
  }
  data->z_buffer = malloc(config.width * sizeof(double));
  if (!data->z_buffer) {
    destroy_impl(data);
    return NULL;
  }
  data->width = config.width;
  data->height = config.height;
  data->screen = SDL_CreateTexture(data->renderer, SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_STREAMING, data->width, data->height);
  r->vtbl = &renderer_vtbl;
  r->impl = data;
  return r;
}

RcRenderer *sdl_renderer_create(RcRendererConfig config)
{
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return NULL;
  }
  if (config.use_gpu) {
    return NULL;
  } else
    return create_software_renderer(config);
}

RcRendererVtbl renderer_vtbl = {
    .render = render, .get_renderer = get_renderer, .get_window = get_window, .destroy = destroy};
