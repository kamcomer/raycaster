#ifndef RAYCASTER_ACTOR_H
#define RAYCASTER_ACTOR_H

#include "input.h"
#include "level.h"
#include <stdbool.h>

typedef struct RcActor RcActor;

typedef struct RcActorVtbl {
  void (*update)(RcActor *actor, AeLevel *level, AeInput *input, float delta_t);
  AeVector (*get_position)(RcActor *actor);
  AeVector (*get_direction)(RcActor *actor);
  uint32_t (*get_texture)(RcActor *actor);
  void (*destroy)(RcActor *actor);
} RcActorVtbl;

struct RcActor {
  const RcActorVtbl *vtbl;
  void *impl;
  AeVector pos;
  AeVector dir;
  AeVector velocity;
  int health;
  uint32_t texture_id;
};

void ae_actor_update(RcActor *actor, AeLevel *world, AeInput *input, float delta_t);
AeVector ae_actor_get_position(RcActor *actor);
AeVector ae_actor_get_direction(RcActor *actor);
uint32_t ae_actor_get_texture(RcActor *actor);
void ae_actor_destory(RcActor *actor);

#endif // RAYCASTER_ACTOR_H
