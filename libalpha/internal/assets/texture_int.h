#ifndef TEXTURE_INT_H
#define TEXTURE_INT_H

#include "raycaster/types.h"
#include <stdlib.h>

typedef size_t AeTextureHandle;
typedef struct AeTextureManager AeTextureManager;
typedef struct AeTexture AeTexture;
typedef struct AeTextureArray AeTextureArray;

struct AeTexture {
  uint32_t *pixels;
  size_t height;
  size_t width;
};

struct AeTextureArray {
  AeTexture *items;
  size_t len;
  size_t capacity;
};

struct AeTextureManager {
  AeTextureArray cache;
};

AeResult ae_texture_load_from_file(const char *file_path, AeTexture *out);

AeResult ae_texture_manager_create(size_t cache_size, AeTextureManager **out);
AeResult ae_texture_array_create(size_t size, AeTextureArray **out);
AeResult ae_texture_create(const uint32_t *pixels, size_t width, size_t height, AeTexture **out);

AeResult ae_texture_manager_init(size_t cache_size, AeTextureManager *out);
AeResult ae_texture_array_init(size_t size, AeTextureArray *out);
AeResult ae_texture_init(const uint32_t *pixels, size_t width, size_t height, AeTexture *out);

void ae_texture_manager_destroy(AeTextureManager *manager);
void ae_texture_array_destroy(AeTextureArray *array);
void ae_texture_destroy(AeTexture *texture);

void ae_texture_manager_deinit(AeTextureManager *manager);
void ae_texture_array_deinit(AeTextureArray *array);
void ae_texture_deinit(AeTexture *texture);

#endif // TEXTURE_INT_H
