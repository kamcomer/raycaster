#ifndef RAYCASTER_LEVEL_H
#define RAYCASTER_LEVEL_H

#include "types.h"

typedef struct RcLevel RcLevel;

typedef struct RcSprite {
  RcVector pos;
  uint32_t texture_id;
  bool is_dynamic;
} RcSprite;

RcLevel *rc_level_load_from_file(const char *file_path);
RcLevel *rc_level_create_empty(uint32_t width, uint32_t height);
uint32_t rc_level_get_width(RcLevel *level);
uint32_t rc_level_get_height(RcLevel *level);
uint32_t rc_level_get_wall(RcLevel *level, int idx_x, int idx_y);
uint32_t rc_level_get_floor(RcLevel *level, int idx_x, int idx_y);
uint32_t rc_level_get_ceil(RcLevel *level, int idx_x, int idx_y);
uint32_t rc_level_get_unit_size(RcLevel *level);
void rc_level_get_sprites(RcLevel *level, RcSprite **out, uint32_t *count);
void rc_level_update(RcLevel *level, float delta_t);
void rc_level_destroy(RcLevel *level);

#endif // RAYCASTER_LEVEL_H
