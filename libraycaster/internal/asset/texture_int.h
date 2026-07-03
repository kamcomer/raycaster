#ifndef TEXTURE_INT_H
#define TEXTURE_INT_H

#include <stdlib.h>

typedef uint32_t RcTextureHandle;

typedef enum RcTextureResult {
  RC_TEXTURE_RESULT_OK,
} RcTextureResult;

typedef struct {
  uint32_t *pixels;
  uint32_t height;
  uint32_t width;
} RcTexture;

typedef struct {
  RcTexture *items;
  uint32_t len;
  uint32_t capacity;
} RcTextureArray;

RcTextureResult rc_texture_load_many(char **paths, size_t count, RcTextureArray *out);
RcTextureResult rc_texture_create_array(size_t size, RcTextureArray *out);
RcTextureResult rc_texture_load(const char *path, RcTexture *out);
void rc_texture_array_destroy(RcTextureArray *array);
void rc_texture_destroy(RcTexture *texture);

#endif
