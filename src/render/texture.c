#include "texture.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

uint32_t *load_image_data(const char *file_path, int *ptr_width, int *ptr_height)
{
  SDL_Surface *texture_surface = IMG_Load(file_path);
  if (!texture_surface)
  {
    fprintf(stderr, "Could not load image: %s\n", SDL_GetError());
    return NULL;
  }

  *ptr_width = texture_surface->w;
  *ptr_height = texture_surface->h;

  // Convert surface to RGBA8888 format
  SDL_Surface *formatted_surface = SDL_ConvertSurface(
      texture_surface, SDL_PIXELFORMAT_RGBA8888);

  SDL_DestroySurface(texture_surface);
  if (!formatted_surface)
  {
    fprintf(stderr, "Could not convert surface to RGBA8888: %s\n",
            SDL_GetError());
    return NULL;
  }

  printf("Loaded surface: %dx%d, format: %d\n", formatted_surface->w,
         formatted_surface->h, formatted_surface->format);

  uint32_t *texture_pixels =
      malloc(formatted_surface->w * formatted_surface->h * sizeof(uint32_t));
  if (!texture_pixels)
  {
    fprintf(stderr, "Could not allocate memory for pixel data\n");
    SDL_DestroySurface(formatted_surface);
    return NULL;
  }

  // Copy the pixel data from the surface to the array
  uint32_t *formatted_pixels = (uint32_t *)formatted_surface->pixels;
  for (int current_row = 0; current_row < formatted_surface->h; current_row++)
  {
    for (int current_col = 0; current_col < formatted_surface->w; current_col++)
    {
      // Get the pixel value and store it in the array
      uint32_t pixel = formatted_pixels[current_row * formatted_surface->w + current_col];
      texture_pixels[current_row * formatted_surface->w + current_col] = pixel;
    }
  }

  SDL_DestroySurface(formatted_surface);

  return texture_pixels;
}

// TODO: have some issues rendering textures with alpha channels
TextureData *create_textures(void)
{
  // int textures[8][DEFAULT_TEXTURE_HEIGHT * DEFAULT_TEXTURE_WIDTH];
  TextureData *textures = malloc(sizeof(TextureData) * 11);
  int width, height;

  // Static Textures
  uint32_t *t0 =
      load_image_data("assets/textures/bluestone.png", &width, &height);
  uint32_t *t1 =
      load_image_data("assets/textures/colorstone.png", &width, &height);
  uint32_t *t2 = load_image_data("assets/textures/eagle.png", &width, &height);
  uint32_t *t3 =
      load_image_data("assets/textures/greystone.png", &width, &height);
  uint32_t *t4 = load_image_data("assets/textures/mossy.png", &width, &height);
  uint32_t *t5 =
      load_image_data("assets/textures/purplestone.png", &width, &height);
  uint32_t *t6 =
      load_image_data("assets/textures/redbrick.png", &width, &height);
  uint32_t *t7 = load_image_data("assets/textures/wood.png", &width, &height);

  // Sprit textures
  uint32_t *t8 =
      load_image_data("assets/textures/barrel.png", &width, &height);
  uint32_t *t9 =
      load_image_data("assets/textures/pillar.png", &width, &height);
  uint32_t *t10 =
      load_image_data("assets/textures/greenlight.png", &width, &height);

  textures[0].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[1].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[2].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[3].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[4].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[5].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[6].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[7].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[8].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[9].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[10].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));

  for (int current_col = 0; current_col < DEFAULT_TEXTURE_WIDTH; current_col++)
  {
    for (int current_row = 0; current_row < DEFAULT_TEXTURE_HEIGHT; current_row++)
    {
      textures[0].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] = t0[DEFAULT_TEXTURE_HEIGHT * current_row + current_col];
      textures[1].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] = t1[DEFAULT_TEXTURE_HEIGHT * current_row + current_col];
      textures[2].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] = t2[DEFAULT_TEXTURE_HEIGHT * current_row + current_col];
      textures[3].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] = t3[DEFAULT_TEXTURE_HEIGHT * current_row + current_col];
      textures[4].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] = t4[DEFAULT_TEXTURE_HEIGHT * current_row + current_col];
      textures[5].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] = t5[DEFAULT_TEXTURE_HEIGHT * current_row + current_col];
      textures[6].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] = t6[DEFAULT_TEXTURE_HEIGHT * current_row + current_col];
      textures[7].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] = t7[DEFAULT_TEXTURE_HEIGHT * current_row + current_col];
      textures[8].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] = t8[DEFAULT_TEXTURE_HEIGHT * current_row + current_col];
      textures[9].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] = t9[DEFAULT_TEXTURE_HEIGHT * current_row + current_col];
      textures[10].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] = t10[DEFAULT_TEXTURE_HEIGHT * current_row + current_col];
    }
  }

  free(t0);
  free(t1);
  free(t2);
  free(t3);
  free(t4);
  free(t5);
  free(t6);
  free(t7);
  free(t8);
  free(t9);
  free(t10);

  return textures;
}

TextureData *createProgrammaticTextures(void)
{
  TextureData *textures = malloc(sizeof(TextureData) * 11);
  textures[0].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[1].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[2].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[3].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[4].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[5].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[6].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[7].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[8].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[9].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  textures[10].pixels = malloc(sizeof(int) * (DEFAULT_TEXTURE_WIDTH * DEFAULT_TEXTURE_HEIGHT));
  for (int current_col = 0; current_col < DEFAULT_TEXTURE_WIDTH; current_col++)
  {
    for (int current_row = 0; current_row < DEFAULT_TEXTURE_HEIGHT; current_row++)
    {
      int xorcolor = (current_col * 256 / DEFAULT_TEXTURE_WIDTH) ^ (current_row * 256 / DEFAULT_TEXTURE_HEIGHT);
      int ycolor = current_row * 256 / DEFAULT_TEXTURE_HEIGHT;
      int xycolor = current_row * 128 / DEFAULT_TEXTURE_HEIGHT + current_col * 128 / DEFAULT_TEXTURE_WIDTH;
      textures[0].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] =
          65536 * 254 *
          (current_col != current_row && current_col != DEFAULT_TEXTURE_WIDTH - current_row); // flat red pixels with black cross
      textures[1].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] =
          xycolor + 256 * xycolor + 65536 * xycolor; // sloped greyscale
      textures[2].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] =
          256 * xycolor + 65536 * xycolor; // sloped yellow gradient
      textures[3].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] =
          xorcolor + 256 * xorcolor + 65536 * xorcolor;                                       // xor greyscale
      textures[4].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] = 256 * xorcolor; // xor green
      textures[5].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] =
          65536 * 192 * (current_col % 16 && current_row % 16);                               // red bricks
      textures[6].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] = 65536 * ycolor; // red gradient
      textures[7].pixels[DEFAULT_TEXTURE_WIDTH * current_row + current_col] =
          128 + 256 * 128 + 65536 * 128; // flat grey pixels
    }
  }
  return textures;
}

void free_texture(TextureData *textures)
{
  for (int i = 0; i < 11; i++)
  {
    free(textures[i].pixels);
  }
  free(textures);
}
