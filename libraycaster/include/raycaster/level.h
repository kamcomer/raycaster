#ifndef RAYCASTER_LEVEL_H
#define RAYCASTER_LEVEL_H

#include "camera.h"
#include "types.h"

typedef struct RcLevel RcLevel;

typedef struct RcSprite {
  RcVector pos;
  int texture_id;
  bool is_dynamic;
} RcSprite;

int rc_level_get_width(RcLevel *w);
int rc_level_get_height(RcLevel *w);
int rc_level_get_wall(RcLevel *w, int x, int y);
int rc_level_get_floor(RcLevel *w, int x, int y);
int rc_level_get_ceil(RcLevel *w, int x, int y);
int rc_level_get_unit_size(RcLevel *w);
void rc_level_get_sprites(RcLevel *w, RcSprite **out, int *count);
void rc_level_update(RcLevel *w, float dt);
void rc_level_destroy(RcLevel *w);

#endif // RAYCASTER_LEVEL_H
