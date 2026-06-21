#ifndef ACTOR_H
#define ACTOR_H

#include "map.h"
#include "vector.h"

#define DEFAULT_NUM_RAYS 200

typedef struct
{
  int size;
  double field_of_view;
  float turn_speed;

  double max_vel;
  double accel;

  Vector pos;
  Vector dir;
  Vector velocity;
  Vector *view_cone;
} Actor;

void cast_actor_rays(Actor *actor, Map map);
void rotate_actor(Actor *actor, float angle);
void move_actor(Actor *actor, int dir, Map map, float frame_time);

// DDA algo
Vector get_ray_row_intersect(Vector origin, Vector ray, Map map);
Vector get_ray_col_intersect(Vector origin, Vector ray, Map map);
#endif
