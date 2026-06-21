#ifndef LEVEL_INT_H
#define LEVEL_INT_H

#include "raycaster/level.h"

typedef struct RcLevelVtbl RcLevelVtbl;

struct RcLevel {
  const RcLevelVtbl *vtbl;
  void *impl;
};

struct RcLevelVtbl {
  int (*width)(RcLevel *w);
  int (*height)(RcLevel *w);
  int (*wall)(RcLevel *w, int x, int y);
  int (*floor)(RcLevel *w, int x, int y);
  int (*ceil)(RcLevel *w, int x, int y);
  int (*unit_size)(RcLevel *w);
  void (*sprites)(RcLevel *w, RcSprite **out, int *count);
  void (*update)(RcLevel *w, float dt);
  void (*destroy)(RcLevel *w);
};

#endif // LEVEL_INT_H
