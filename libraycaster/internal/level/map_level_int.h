#ifndef MAP_LEVEL_INT_H
#define MAP_LEVEL_INT_H

#include "raycaster/level.h"
#include "level_int.h"
#include <stdint.h>

#define DEFAULT_MAP_UNIT_SIZE 20
#define MAX_TEXTURE_PATHS 11

typedef enum {
  MAP_SECTION_NONE,
  MAP_SECTION_MAP,
  MAP_SECTION_CEIL,
  MAP_SECTION_FLOOR,
  MAP_SECTION_TEXTURES,
  MAP_SECTION_SPRITES
} MapSection;

typedef struct {
  uint8_t **walls;
  uint8_t **ceil;
  uint8_t **floor;
  int unit_size;
  int width;
  int height;
  RcSprite *sprites;
  int sprite_count;
  char *texture_paths[MAX_TEXTURE_PATHS];
  int texture_count;
} MapLevelData;

extern RcLevelVtbl maplevel_vtbl;

#endif // MAP_LEVEL_INT_H
