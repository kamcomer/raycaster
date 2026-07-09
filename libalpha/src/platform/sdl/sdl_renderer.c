#include "alpha.h"
#include "internal/i_alpha.h"
#include "internal/platform/sdl/sdl_renderer_int.h"
#include <stdlib.h>

static void sdl_renderer_backend_data_deinit(SdlRendererBackendData *data)
{
  SDL_DestroyTexture(data->screen);
  SDL_DestroyRenderer(data->renderer);
  SDL_DestroyWindow(data->window);
  free(data->framebuffer);
  free(data->z_buffer);
  SDL_Quit();
}

static void sdl_renderer_backend_data_destroy(SdlRendererBackendData *data)
{
  sdl_renderer_backend_data_deinit(data);
  free(data);
}

static AResult sdl_renderer_backend_data_init(ARendererConfig config, SdlRendererBackendData *out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = (SdlRendererBackendData){0};

  out->config = config;

  out->window = SDL_CreateWindow("", config.width, config.height, 0);
  if (!out->window) {
    fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
    sdl_renderer_backend_data_destroy(out);
    *out = (SdlRendererBackendData){0};
    return A_RES_BACKEND_ERR;
  }

  out->window_dimensions = malloc(sizeof(ADimensions));
  if (!out->window) {
    sdl_renderer_backend_data_destroy(out);
    *out = (SdlRendererBackendData){0};
    return A_RES_ALLOC_ERR;
  }
  SDL_GetWindowSize(out->window, (int *)&out->window_dimensions->width,
                    (int *)&out->window_dimensions->height);

  out->renderer = SDL_CreateRenderer(out->window, NULL);
  if (!out->renderer) {
    fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
    sdl_renderer_backend_data_destroy(out);
    *out = (SdlRendererBackendData){0};
    return A_RES_BACKEND_ERR;
  }

  out->screen =
      SDL_CreateTexture(out->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                        out->window_dimensions->width, out->window_dimensions->height);
  if (!out->screen) {
    fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
    sdl_renderer_backend_data_destroy(out);
    *out = (SdlRendererBackendData){0};
    return A_RES_BACKEND_ERR;
  }

  out->framebuffer = malloc(config.width * config.height * sizeof(uint32_t));
  if (!out->framebuffer) {
    sdl_renderer_backend_data_destroy(out);
    *out = (SdlRendererBackendData){0};
    return A_RES_ALLOC_ERR;
  }
  out->z_buffer = malloc(config.width * sizeof(double));
  if (!out->z_buffer) {
    sdl_renderer_backend_data_destroy(out);
    *out = (SdlRendererBackendData){0};
    return A_RES_ALLOC_ERR;
  }

  return A_RES_OK;
}

static AResult sdl_renderer_backend_data_create(ARendererConfig config,
                                                SdlRendererBackendData **out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = NULL;
  SdlRendererBackendData *data = malloc(sizeof(SdlRendererBackendData));
  if (!data) {
    return A_RES_ALLOC_ERR;
  }

  AResult res = sdl_renderer_backend_data_init(config, data);
  if (res != A_RES_OK) {
    free(data);
    return A_RES_ALLOC_ERR;
  }

  *out = data;
  return A_RES_OK;
}

static void destroy(ARendererBackend *backend) { sdl_renderer_backend_data_destroy(backend->impl); }

static void render(const ARendererBackend *backend)
{
  int width;
  int height;
  SdlRendererBackendData *data = backend->impl;
  SDL_GetWindowSize(data->window, &width, &height);
  SDL_UpdateTexture(data->screen, NULL, data->framebuffer, width * 4);
  SDL_RenderClear(data->renderer);
  SDL_RenderTexture(data->renderer, data->screen, NULL, NULL);
  SDL_RenderPresent(data->renderer);
}

static uint32 *get_framebuffer(const ARendererBackend *backend)
{
  SdlRendererBackendData *data = backend->impl;
  return data->framebuffer;
}

static double *get_zbuffer(const ARendererBackend *backend)
{
  SdlRendererBackendData *data = backend->impl;
  return data->z_buffer;
}

static ADimensions *get_window_dimensions(const ARendererBackend *backend)
{
  SdlRendererBackendData *data = backend->impl;
  return data->window_dimensions;
}

ARendererBackendVtbl renderer_backend_vtbl = {.render = render,
                                              .get_framebuffer = get_framebuffer,
                                              .get_zbuffer = get_zbuffer,
                                              .get_window_diminsions = get_window_dimensions,
                                              .destroy = destroy};

AResult sdl_renderer_init(ARendererConfig config, ARendererBackend *out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return A_RES_BACKEND_ERR;
  }

  *out = (ARendererBackend){0};

  AResult res = sdl_renderer_backend_data_create(config, (SdlRendererBackendData **)&out->impl);
  if (res != A_RES_OK) {
    *out = (ARendererBackend){0};
    return res;
  }

  out->vtbl = &renderer_backend_vtbl;
  return A_RES_OK;
}

AResult sdl_renderer_create(ARendererConfig config, ARendererBackend **out)
{

  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = NULL;

  ARendererBackend *backend = malloc(sizeof(ARendererBackend));
  if (!backend) {
    return A_RES_ALLOC_ERR;
  }

  AResult res = sdl_renderer_init(config, backend);
  if (res != A_RES_OK) {
    return res;
  }

  *out = backend;

  return A_RES_OK;
}
