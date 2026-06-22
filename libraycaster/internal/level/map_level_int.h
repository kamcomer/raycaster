#ifndef MAP_LEVEL_INT_H
#define MAP_LEVEL_INT_H

#include "internal/level/level_int.h"
#include "raycaster/level.h"
#include <stdint.h>

#define DEFAULT_MAP_UNIT_SIZE 20
#define MAX_TEXTURE_PATHS 11

typedef enum {
  MAP_SECTION_NONE,
  MAP_SECTION_MAP,
  MAP_SECTION_CEIL,
  MAP_SECTION_FLOOR,
  MAP_SECTION_TEXTURES,
  MAP_SECTION_SPRITE_TYPES,
  MAP_SECTION_SPRITES
} MapSection;

#define MAX_SPRITE_TYPES 16
#define MAX_SPRITE_TYPE_KEY_LEN 64

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
  char sprite_type_keys[MAX_SPRITE_TYPES][MAX_SPRITE_TYPE_KEY_LEN];
  char *sprite_type_paths[MAX_SPRITE_TYPES];
  int sprite_type_count;
} MapLevelData;

extern RcLevelVtbl maplevel_vtbl;

#endif // MAP_LEVEL_INT_H
