#ifndef RAYCASTER_ACTOR_H
#define RAYCASTER_ACTOR_H

#include "input.h"
#include "level.h"
#include "types.h"

typedef struct RcActor RcActor;

typedef struct RcActorVtbl {
  void (*update)(RcActor *actor, RcLevel *level, RcInput *input, float delta_t);
  RcVector (*get_position)(RcActor *actor);
  RcVector (*get_direction)(RcActor *actor);
  uint32_t (*get_texture)(RcActor *actor);
  void (*destroy)(RcActor *actor);
} RcActorVtbl;

struct RcActor {
  const RcActorVtbl *vtbl;
  RcVector pos;
  RcVector dir;
  RcVector velocity;
  int health;
  uint32_t texture_id;
  void *impl;
};

void rc_actor_update(RcActor *actor, RcLevel *world, RcInput *input, float delta_t);
RcVector rc_actor_get_position(RcActor *actor);
RcVector rc_actor_get_direction(RcActor *actor);
uint32_t rc_actor_get_texture(RcActor *actor);
void rc_actor_destroy(RcActor *actor);

#endif // RAYCASTER_ACTOR_H
