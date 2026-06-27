#ifndef TEXTURE_INT_H
#define TEXTURE_INT_H

#include <stdlib.h>

typedef struct {
  char *path;
  uint32_t *pixels;
  uint32_t h;
  uint32_t w;
} RcTexture;

typedef struct {
  RcTexture *items;
  uint32_t len;
  uint32_t capacity;
} RcTextureArray;

RcTextureArray *rc_load_textures(char **paths, size_t count);
RcTextureArray *rc_create_texture_array(size_t size);
int rc_load_texture(const char *path, RcTexture *td);
void rc_free_textures(RcTextureArray *ta);
void rc_free_texture(RcTexture *texture);

#endif
