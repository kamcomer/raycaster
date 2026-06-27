#ifndef LEVEL_INT_H
#define LEVEL_INT_H

#include "internal/util/general.h"
#include "raycaster/level.h"
#include "raycaster/texture.h"

typedef struct RcLevelVtbl RcLevelVtbl;

struct RcLevel {
  const RcLevelVtbl *vtbl;
  void *impl;
};

struct RcLevelVtbl {
  uint32_t (*width)(RcLevel *w);
  uint32_t (*height)(RcLevel *w);
  uint32_t (*wall)(RcLevel *w, int x, int y);
  uint32_t (*floor)(RcLevel *w, int x, int y);
  uint32_t (*ceil)(RcLevel *w, int x, int y);
  uint32_t (*unit_size)(RcLevel *w);
  StringArray *(*texture_paths)(RcLevel *w);
  void (*sprites)(RcLevel *w, RcSprite **out, uint32_t *count);
  void (*update)(RcLevel *w, float dt);
  void (*destroy)(RcLevel *w);
};

StringArray *level_get_texture_paths(RcLevel *w);

#endif // LEVEL_INT_H
