#ifndef RAYCASTER_LEVEL_H
#define RAYCASTER_LEVEL_H

#include "camera.h"
#include "types.h"

typedef struct RcLevel RcLevel;

typedef struct RcSprite {
  RcVector pos;
  uint32_t texture_id;
  bool is_dynamic;
} RcSprite;

RcLevel *rc_level_load_from_file(const char *file_path);
RcLevel *rc_level_create_empty(uint32_t width, uint32_t height);

uint32_t rc_level_get_width(RcLevel *w);
uint32_t rc_level_get_height(RcLevel *w);
uint32_t rc_level_get_wall(RcLevel *w, int x, int y);
uint32_t rc_level_get_floor(RcLevel *w, int x, int y);
uint32_t rc_level_get_ceil(RcLevel *w, int x, int y);
uint32_t rc_level_get_unit_size(RcLevel *w);
void rc_level_get_sprites(RcLevel *w, RcSprite **out, uint32_t *count);
void rc_level_update(RcLevel *w, float dt);
void rc_level_destroy(RcLevel *w);

#endif // RAYCASTER_LEVEL_H
