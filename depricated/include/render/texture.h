#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

#define DEFAULT_TEXTURE_WIDTH 64
#define DEFAULT_TEXTURE_HEIGHT 64

typedef struct
{
  int *pixels;
  int height;
  int width;
} TextureData;

TextureData *create_textures(void);
uint32_t *load_image_data(const char *file_path, int *ptr_width, int *ptr_height);
void free_texture(TextureData *texture);

#endif
