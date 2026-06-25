#ifndef LEVEL_INT_H
#define LEVEL_INT_H

#include "raycaster/level.h"

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
  void (*sprites)(RcLevel *w, RcSprite **out, uint32_t *count);
  void (*update)(RcLevel *w, float dt);
  void (*destroy)(RcLevel *w);
};

#endif // LEVEL_INT_H
