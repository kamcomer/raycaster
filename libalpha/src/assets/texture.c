#include "internal/i_alpha.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string.h>

AResult a_texture_load_from_file(const char *file_path, ATexture *out)
{
  SDL_Surface *texture_surface = IMG_Load(file_path);
  if (!texture_surface) {
    fprintf(stderr, "Could not load image: %s\n", SDL_GetError());
    return A_RES_BACKEND_ERR;
  }

  SDL_Surface *formatted_surface = SDL_ConvertSurface(texture_surface, SDL_PIXELFORMAT_RGBA8888);
  SDL_DestroySurface(texture_surface);
  if (!formatted_surface) {
    fprintf(stderr, "Could not convert surface to RGBA8888: %s\n", SDL_GetError());
    return A_RES_BACKEND_ERR;
  }

  AResult res = a_texture_init((uint32_t *)formatted_surface->pixels, formatted_surface->w,
                               formatted_surface->h, out);

  SDL_DestroySurface(formatted_surface);

  return res;
}

AResult a_texture_manager_create(size_t cache_size, ATextureManager **out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = NULL;

  ATextureManager *texture_manager = malloc(sizeof(ATextureManager));
  if (!texture_manager) {
    return A_RES_ALLOC_ERR;
  }

  AResult res = a_texture_manager_init(cache_size, texture_manager);
  if (res != A_RES_OK) {
    free(texture_manager);
    return res;
  }

  return A_RES_OK;
}

AResult a_texture_manager_init(size_t cache_size, ATextureManager *out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = (ATextureManager){0};

  AResult res = a_texture_array_init(cache_size, &out->cache);
  if (res != A_RES_OK) {
    *out = (ATextureManager){0};
    return A_RES_ALLOC_ERR;
  }
  return A_RES_OK;
}

void a_texture_manager_destroy(ATextureManager *manager)
{
  if (!manager) {
    return;
  }
  a_texture_manager_deinit(manager);
  free(manager);
}

void a_texture_manager_deinit(ATextureManager *manager)
{
  if (!manager) {
    return;
  }
  a_texture_array_destroy(&manager->cache);
  manager->cache = (ATextureArray){0};
}

AResult a_texture_array_create(size_t size, ATextureArray **out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = NULL;

  ATextureArray *array = malloc(sizeof(ATextureArray));
  if (!array) {
    return A_RES_ALLOC_ERR;
  }

  AResult res = a_texture_array_init(size, array);
  if (res != A_RES_OK) {
    free(array);
    return res;
  }

  *out = array;
  return A_RES_OK;
}

AResult a_texture_array_init(size_t size, ATextureArray *out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = (ATextureArray){0};

  ATexture *items = malloc(size * sizeof(ATexture));
  if (!items) {
    *out = (ATextureArray){0};
    return A_RES_ALLOC_ERR;
  }
  out->items = items;
  out->capacity = size;
  out->len = 0;

  return A_RES_OK;
}

void a_texture_array_destroy(ATextureArray *array)
{
  if (!array) {
    return;
  }

  a_texture_array_deinit(array);
  free(array);
}

void a_texture_array_deinit(ATextureArray *array)
{
  if (!array) {
    return;
  }

  free(array->items);
  *array = (ATextureArray){0};
}

AResult a_texture_create(const uint32_t *pixels, size_t width, size_t height, ATexture **out)
{
  if (!out || !pixels) {
    return A_RES_INVLD_ARG;
  }

  *out = NULL;

  ATexture *texture = malloc(sizeof(texture));
  if (!texture) {
    return A_RES_ALLOC_ERR;
  }

  AResult res = a_texture_init(pixels, width, height, texture);
  if (res != A_RES_OK) {
    free(texture);
    return res;
  }

  *out = texture;

  return A_RES_OK;
}

AResult a_texture_init(const uint32_t *pixels, int width, int height, ATexture *out)
{
  if (!out || !pixels) {
    return A_RES_INVLD_ARG;
  }

  *out = (ATexture){0};

  out->pixels = malloc(width * height * sizeof(uint32_t));
  if (!out->pixels) {
    *out = (ATexture){0};
    return A_RES_ALLOC_ERR;
  }
  memcpy(out->pixels, (void *)pixels, width * height * sizeof(uint32));
  if (!out->pixels) {
    *out = (ATexture){0};
    return A_RES_ALLOC_ERR;
  }
  out->width = width;
  out->height = height;

  return A_RES_OK;
}

void a_texture_destroy(ATexture *texture)
{
  if (!texture) {
    return;
  }
  a_texture_deinit(texture);
  free(texture);
}

void a_texture_deinit(ATexture *texture)
{
  if (!texture) {
    return;
  }
  free(texture->pixels);
  texture->pixels = NULL;
  texture->width = 0;
  texture->height = 0;
}
