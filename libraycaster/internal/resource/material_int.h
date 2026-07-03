#ifndef MATERIAL_INT_H
#define MATERIAL_INT_H

#include "internal/resource/texture_int.h"
#include <stdint.h>

typedef struct AeMaterial AeMaterial;
typedef struct AeMaterialArray AeMaterialArray;

struct AeMaterial {
  AeTextureHandle texture;
};

struct AeMaterialArray {
  AeMaterial *items;
  size_t len;
  size_t capacity;
};

#endif // MATERIAL_INT_H
