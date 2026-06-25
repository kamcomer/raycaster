#ifndef RAYCASTER_TYPES_H
#define RAYCASTER_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#define RC_PI 3.14159265358979323846
#define RC_PI_2 (RC_PI / 2.0)
#define RC_DEG_TO_RAD (RC_PI / 180.0)

#define RC_TEXTURE_WIDTH 64
#define RC_TEXTURE_HEIGHT 64

typedef struct
{
    double x;
    double y;
    double mag;
    double angle;
} RcVector;

typedef struct
{
    int x;
    int y;
    uint32_t w;
    uint32_t h;
} RcRect;

typedef struct
{
    uint32_t width;
    uint32_t height;
} RcDimensions;

typedef struct
{
    uint32_t *pixels;
    uint32_t height;
    uint32_t width;
} RcTextureData;

#endif // RAYCASTER_TYPES_H
