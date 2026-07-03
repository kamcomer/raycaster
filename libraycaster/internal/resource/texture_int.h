#ifndef TEXTURE_INT_H
#define TEXTURE_INT_H

#include <stdlib.h>

typedef size_t AeTextureHandle;
typedef struct AeTextureManager AeTextureManager;
typedef struct AeTexture AeTexture;
typedef struct AeTextureArray AeTextureArray;

typedef enum AeTextureResult {
  AE_TEXTURE_RES_OK,
  AE_TEXTURE_RES_INVD_ARG,
  AE_TEXTURE_RES_ALLOC_ERR,
  AE_TEXTURE_RES_FAILED
} AeTextureResult;

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

AeTextureResult ae_texture_load_from_file(const char *file_path, AeTexture *out);
AeTextureResult ae_texture_manager_create(size_t cache_size, AeTextureManager *out);
AeTextureResult ae_texture_create_array(size_t size, AeTextureArray *out);
AeTextureResult ae_texture_create(const uint32_t *pixels, size_t width, size_t height,
                                  AeTexture *out);
void ae_texture_array_destroy(AeTextureArray *array);
void ae_texture_destroy(AeTexture *texture);
void ae_texture_manager_destroy(AeTextureManager *manager);

#endif // TEXTURE_INT_H
