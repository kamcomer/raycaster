#ifndef LEVEL_INT_H
#define LEVEL_INT_H

#include "kutils/str.h"
#include "raycaster/level.h"

typedef struct RcLevelVtbl RcLevelVtbl;

struct RcLevel {
  const RcLevelVtbl *vtbl;
  void *impl;
};

struct RcLevelVtbl {
  uint32_t (*width)(RcLevel *level);
  uint32_t (*height)(RcLevel *level);
  uint32_t (*wall)(RcLevel *level, int idx_x, int idx_y);
  uint32_t (*floor)(RcLevel *level, int idx_x, int idx_y);
  uint32_t (*ceil)(RcLevel *level, int idx_x, int idx_y);
  uint32_t (*unit_size)(RcLevel *level);
  StringArray *(*texture_paths)(RcLevel *level);
  void (*sprites)(RcLevel *level, RcSprite **out, uint32_t *count);
  void (*update)(RcLevel *level, float delta_t);
  void (*destroy)(RcLevel *level);
};

StringArray *level_get_texture_paths(RcLevel *level);

#endif // LEVEL_INT_H
