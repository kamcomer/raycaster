#include "raycaster/texture.h"
#include "internal/util/general.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

void rc_free_textures(RcTextureArray *ta)
{
  if (!ta)
    return;
  for (uint32_t i = 0; i < ta->len; i++) {
    rc_free_texture(&(ta->items[i]));
  }
  free(ta);
}

void rc_free_texture(RcTexture *t)
{
  if (!t) {
    return;
  }
  if (t->pixels)
    free(t->pixels);
  free(t);
}

int rc_load_texture(const char *file_path, RcTexture *texture)
{
  SDL_Surface *texture_surface = IMG_Load(file_path);
  if (!texture_surface) {
    fprintf(stderr, "Could not load image: %s\n", SDL_GetError());
    return -1;
  }

  SDL_Surface *formatted_surface = SDL_ConvertSurface(texture_surface, SDL_PIXELFORMAT_RGBA8888);
  SDL_DestroySurface(texture_surface);
  if (!formatted_surface) {
    fprintf(stderr, "Could not convert surface to RGBA8888: %s\n", SDL_GetError());
    return -1;
  }

  uint32_t w = formatted_surface->w;
  uint32_t h = formatted_surface->h;

  texture->w = w;
  texture->h = h;

  if (texture->path)
    free(texture->path);

  texture->path = strdup(file_path);

  if (texture->pixels)
    free(texture->pixels);

  texture->pixels = calloc(w * h, sizeof(uint32_t));
  if (!texture->pixels) {
    fprintf(stderr, "Could not allocate memory for pixel data\n");
    SDL_DestroySurface(formatted_surface);
    return -1;
  }

  uint32_t *pixels = (uint32_t *)formatted_surface->pixels;
  for (uint32_t row = 0; row < h; row++) {
    for (uint32_t col = 0; col < w; col++) {
      texture->pixels[row * w + col] = pixels[row * w + col];
    }
  }

  SDL_DestroySurface(formatted_surface);

  return 0;
}

RcTextureArray *rc_load_textures(char **paths, size_t count)
{

  RcTextureArray *textures = rc_create_texture_array(count);

  // const char *paths[] = {
  //     "assets/textures/bluestone.png", "assets/textures/colorstone.png",
  //     "assets/textures/eagle.png",     "assets/textures/greystone.png",
  //     "assets/textures/mossy.png",     "assets/textures/purplestone.png",
  //     "assets/textures/redbrick.png",  "assets/textures/wood.png",
  //     "assets/sprites/barrel.png",     "assets/sprites/pillar.png",
  //     "assets/sprites/greenlight.png",
  // };

  for (int i = 0; i < count; i++) {
    if (rc_load_texture(paths[i], &(textures->items[i])) != 0) {
      rc_free_textures(textures);
      return NULL;
    }
  }

  return textures;
}

RcTextureArray *rc_create_texture_array(size_t size)
{
  RcTextureArray *ta = calloc(1, sizeof(RcTextureArray));
  if (!ta) {
    return NULL;
  }

  ta->items = calloc(size, sizeof(RcTexture));
  if (!ta->items) {
    free(ta);
    return NULL;
  }
  ta->capacity = size;

  return ta;
}