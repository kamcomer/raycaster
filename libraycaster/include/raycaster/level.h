#ifndef RAYCASTER_LEVEL_H
#define RAYCASTER_LEVEL_H

#include "types.h"

typedef struct RcLevel AeLevel;

typedef struct RcSprite {
  AeVector pos;
  uint32_t texture_id;
  bool is_dynamic;
} RcSprite;

AeLevel *rc_level_load_from_file(const char *file_path);
AeLevel *rc_level_create_empty(uint32_t width, uint32_t height);
uint32_t rc_level_get_width(AeLevel *level);
uint32_t rc_level_get_height(AeLevel *level);
uint32_t rc_level_get_wall(AeLevel *level, int idx_x, int idx_y);
uint32_t rc_level_get_floor(AeLevel *level, int idx_x, int idx_y);
uint32_t rc_level_get_ceil(AeLevel *level, int idx_x, int idx_y);
uint32_t rc_level_get_unit_size(AeLevel *level);
void rc_level_get_sprites(AeLevel *level, RcSprite **out, uint32_t *count);
void rc_level_update(AeLevel *level, float delta_t);
void ae_level_destroy(AeLevel *level);

#endif // RAYCASTER_LEVEL_H
