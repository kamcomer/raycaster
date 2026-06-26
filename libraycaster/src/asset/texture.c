#include "internal/asset/texture_int.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

void rc_free_textures(RcTextureData *textures)
{
  if (!textures)
    return;
  for (int i = 0; i < 11; i++) {
    free(textures[i].pixels);
  }
  free(textures);
}

static uint32_t *load_image_data(const char *file_path, uint32_t *ptr_width, uint32_t *ptr_height)
{
  SDL_Surface *texture_surface = IMG_Load(file_path);
  if (!texture_surface) {
    fprintf(stderr, "Could not load image: %s\n", SDL_GetError());
    return NULL;
  }

  *ptr_width = (uint32_t)texture_surface->w;
  *ptr_height = (uint32_t)texture_surface->h;

  SDL_Surface *formatted_surface = SDL_ConvertSurface(texture_surface, SDL_PIXELFORMAT_RGBA8888);

  SDL_DestroySurface(texture_surface);
  if (!formatted_surface) {
    fprintf(stderr, "Could not convert surface to RGBA8888: %s\n", SDL_GetError());
    return NULL;
  }

  uint32_t *texture_pixels = malloc(formatted_surface->w * formatted_surface->h * sizeof(uint32_t));
  if (!texture_pixels) {
    fprintf(stderr, "Could not allocate memory for pixel data\n");
    SDL_DestroySurface(formatted_surface);
    return NULL;
  }

  uint32_t *formatted_pixels = (uint32_t *)formatted_surface->pixels;
  for (int current_row = 0; current_row < formatted_surface->h; current_row++) {
    for (int current_col = 0; current_col < formatted_surface->w; current_col++) {
      uint32_t pixel = formatted_pixels[current_row * formatted_surface->w + current_col];
      texture_pixels[current_row * formatted_surface->w + current_col] = pixel;
    }
  }

  SDL_DestroySurface(formatted_surface);

  return texture_pixels;
}

typedef struct {
  uint32_t *pixels;
  uint32_t width;
  uint32_t height;
} SourceImage;

RcTextureData *rc_create_textures(void)
{
  RcTextureData *textures = malloc(sizeof(RcTextureData) * 11);
  if (!textures)
    return NULL;

  SourceImage sources[11];
  const char *paths[] = {
      "assets/textures/bluestone.png",   "assets/textures/colorstone.png",
      "assets/textures/eagle.png",       "assets/textures/greystone.png",
      "assets/textures/mossy.png",       "assets/textures/purplestone.png",
      "assets/textures/redbrick.png",    "assets/textures/wood.png",
      "assets/sprites/barrel.png",       "assets/sprites/pillar.png",
      "assets/sprites/greenlight.png",
  };

  for (int i = 0; i < 11; i++) {
    sources[i].pixels = load_image_data(paths[i], &sources[i].width, &sources[i].height);
  }

  for (int i = 0; i < 11; i++) {
    textures[i].pixels = calloc(RC_TEXTURE_WIDTH * RC_TEXTURE_HEIGHT, sizeof(uint32_t));
    textures[i].width = RC_TEXTURE_WIDTH;
    textures[i].height = RC_TEXTURE_HEIGHT;
  }

  for (int i = 0; i < 11; i++) {
    if (sources[i].pixels) {
      uint32_t copy_count = sources[i].width * sources[i].height;
      if (copy_count > RC_TEXTURE_WIDTH * RC_TEXTURE_HEIGHT)
        copy_count = RC_TEXTURE_WIDTH * RC_TEXTURE_HEIGHT;
      for (uint32_t j = 0; j < copy_count; j++) {
        textures[i].pixels[j] = sources[i].pixels[j];
      }
      free(sources[i].pixels);
    }
  }

  return textures;
}
