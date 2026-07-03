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
  uint32_t (*width)(AeLevel *level);
  uint32_t (*height)(AeLevel *level);
  uint32_t (*wall)(AeLevel *level, int idx_x, int idx_y);
  uint32_t (*floor)(AeLevel *level, int idx_x, int idx_y);
  uint32_t (*ceil)(AeLevel *level, int idx_x, int idx_y);
  uint32_t (*unit_size)(AeLevel *level);
  StringArray *(*texture_paths)(AeLevel *level);
  void (*sprites)(AeLevel *level, RcSprite **out, uint32_t *count);
  void (*update)(AeLevel *level, float delta_t);
  void (*destroy)(AeLevel *level);
};

StringArray *level_get_texture_paths(AeLevel *level);

#endif // LEVEL_INT_H
