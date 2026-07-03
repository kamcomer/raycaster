#include "internal/resource/texture_int.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string.h>

AeTextureResult ae_texture_load_from_file(const char *file_path, AeTexture *out)
{
  SDL_Surface *texture_surface = IMG_Load(file_path);
  if (!texture_surface) {
    fprintf(stderr, "Could not load image: %s\n", SDL_GetError());
    return AE_TEXTURE_RES_FAILED;
  }

  SDL_Surface *formatted_surface = SDL_ConvertSurface(texture_surface, SDL_PIXELFORMAT_RGBA8888);
  SDL_DestroySurface(texture_surface);
  if (!formatted_surface) {
    fprintf(stderr, "Could not convert surface to RGBA8888: %s\n", SDL_GetError());
    return AE_TEXTURE_RES_FAILED;
  }

  AeTextureResult res = ae_texture_create((uint32_t *)formatted_surface->pixels,
                                          formatted_surface->w, formatted_surface->h, out);

  SDL_DestroySurface(formatted_surface);

  return res;
}

AeTextureResult ae_texture_manager_create(size_t cache_size, AeTextureManager *out)
{
  if (!out) {
    return AE_TEXTURE_RES_INVD_ARG;
  }

  if (out->cache.items) {
    free(out->cache.items);
  }

  return ae_texture_create_array(cache_size, &out->cache);
}

void ae_texture_manager_destroy(AeTextureManager *manager)
{
  if (!manager) {
    return;
  }
  ae_texture_array_destroy(&manager->cache);
}

AeTextureResult ae_texture_create_array(size_t size, AeTextureArray *out)
{
  if (!out) {
    return AE_TEXTURE_RES_INVD_ARG;
  }

  if (out->items) {
    free(out->items);
  }

  out->items = malloc(size * sizeof(AeTexture));
  if (!out->items) {
    return AE_TEXTURE_RES_ALLOC_ERR;
  }
  out->len = 0;
  out->capacity = size;

  return AE_TEXTURE_RES_OK;
}

void ae_texture_array_destroy(AeTextureArray *array)
{
  if (!array) {
    return;
  }

  if (array->items) {
    free(array->items);
  }
  array->items = NULL;
  array->len = 0;
  array->capacity = 0;
}

AeTextureResult ae_texture_create(uint32_t *pixels, size_t width, size_t height, AeTexture *out)
{
  if (!out || !pixels) {
    return AE_TEXTURE_RES_INVD_ARG;
  }
  memcpy(pixels, &out->pixels, width * height * sizeof(uint32_t));
  if (!out->pixels) {
    return AE_TEXTURE_RES_ALLOC_ERR;
  }
  out->width = width;
  out->height = height;
  return AE_TEXTURE_RES_OK;
}

void ae_texture_destroy(AeTexture *texture)
{
  if (!texture) {
    return;
  }
  if (texture->pixels) {
    free(texture->pixels);
  }
  texture->pixels = NULL;
  texture->width = 0;
  texture->height = 0;
}
