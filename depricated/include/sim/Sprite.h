#ifndef SPRITE_H
#define SPRITE_H

#include "actor.h"
#include "vector.h"

typedef struct {
  Vector pos;
  int texture;
} StaticSprite;

typedef struct {
  Actor actor;
  int texture;
} DynamicSprite;

void sort_sprites(int *s_sprite_order, double *s_sprit_dist, int num_s_sprites);
#endif
