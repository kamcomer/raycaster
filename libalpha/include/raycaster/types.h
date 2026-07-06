#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef enum AeResult {
  AE_RES_OK,
  AE_RES_INVLD_ARG,
  AE_RES_ALLOC_ERR,
  AE_RES_BACKEND_ERR,
} AeResult;

typedef struct {
  double x;
  double y;
  double mag;
  double angle;
} AeVector;

typedef struct {
  int x;
  int y;
  uint32_t w;
  uint32_t h;
} AeRect;

typedef struct {
  uint32_t width;
  uint32_t height;
} AeDimensions;

#endif // TYPES_H
