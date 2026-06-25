#ifndef MAP_LEVEL_INT_H
#define MAP_LEVEL_INT_H

#include "internal/level/level_int.h"
#include "internal/util/general.h"
#include "raycaster/level.h"
#include <stdint.h>
#include <stdlib.h>

#define DEFAULT_MAP_UNIT_SIZE 20
#define DEFAULT_NUM_ASSETS 10

typedef enum {
  MAP_SECTION_NONE,
  MAP_SECTION_MAP,
  MAP_SECTION_CEIL,
  MAP_SECTION_FLOOR,
  MAP_SECTION_TEXTURES,
  MAP_SECTION_SPRITE_TYPES,
  MAP_SECTION_SPRITES
} LevelFileSection;

typedef struct {
  char *path;
  uint32_t frame_count;
  float frame_delay;
} SpriteTypeDef;

typedef struct {
  SpriteTypeDef *items;
  uint32_t len;
  uint32_t capacity;
} SpriteTypes;

typedef struct {
  RcSprite *items;
  uint32_t len;
  uint32_t capacity;
} SpriteArray;

typedef struct {
  uint8_t **walls;
  uint8_t **ceil;
  uint8_t **floor;
  size_t unit_size;
  size_t width;
  size_t height;

  StringArray tex_paths;
  SpriteTypes sprite_types;
  SpriteArray sprites;
} MapLevelData;

extern RcLevelVtbl maplevel_vtbl;

#endif // MAP_LEVEL_INT_H
