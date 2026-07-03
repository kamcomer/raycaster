#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>

#define RC_PI 3.14159265358979323846
#define RC_PI_2 (RC_PI / 2.0)
#define RC_DEG_TO_RAD (RC_PI / 180.0)

#define RC_TEXTURE_WIDTH 64
#define RC_TEXTURE_HEIGHT 64

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
