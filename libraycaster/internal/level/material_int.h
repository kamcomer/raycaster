#ifndef MATERIAL_INT_H
#define MATERIAL_INT_H

#include "internal/asset/texture_int.h"
#include <stdint.h>

typedef struct RcMaterial RcMaterial;
typedef struct RcMaterialArray RcMaterialArray;

struct RcMaterial {
  RcTextureHandle texture;
};

struct RcMaterialArray {
  RcMaterial *items;
  size_t len;
  size_t capacity;
};

#endif // MATERIAL_INT_H
