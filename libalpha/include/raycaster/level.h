#ifndef RAYCASTER_LEVEL_H
#define RAYCASTER_LEVEL_H

#include "types.h"
#include <stdbool.h>

typedef struct AeLevel AeLevel;

typedef struct AeSprite {
  AeVector pos;
  uint32_t texture_id;
  bool is_dynamic;
} AeSprite;

AeLevel *ae_level_load_from_file(const char *file_path);
AeLevel *ae_level_create_empty(uint32_t width, uint32_t height);
uint32_t ae_level_get_width(AeLevel *level);
uint32_t ae_level_get_height(AeLevel *level);
uint32_t ae_level_get_wall(AeLevel *level, int idx_x, int idx_y);
uint32_t ae_level_get_floor(AeLevel *level, int idx_x, int idx_y);
uint32_t ae_level_get_ceil(AeLevel *level, int idx_x, int idx_y);
uint32_t ae_level_get_unit_size(AeLevel *level);
void ae_level_get_sprites(AeLevel *level, AeSprite **out, uint32_t *count);
void ae_level_update(AeLevel *level, float delta_t);
void ae_level_destroy(AeLevel *level);

#endif // RAYCASTER_LEVEL_H
