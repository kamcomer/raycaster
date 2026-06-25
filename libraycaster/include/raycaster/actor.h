#ifndef RAYCASTER_ACTOR_H
#define RAYCASTER_ACTOR_H

#include "types.h"
#include "level.h"
#include "input.h"

typedef struct RcActor RcActor;

typedef struct RcActorVtbl
{
    void (*update)(RcActor *a, RcLevel *world, RcInput *input, float dt);
    RcVector (*get_position)(RcActor *a);
    RcVector (*get_direction)(RcActor *a);
    uint32_t (*get_texture)(RcActor *a);
    void (*destroy)(RcActor *a);
} RcActorVtbl;

struct RcActor
{
    const RcActorVtbl *vtbl;
    RcVector pos;
    RcVector dir;
    RcVector velocity;
    int health;
    uint32_t texture_id;
    void *impl;
};

void rc_actor_update(RcActor *a, RcLevel *world, RcInput *input, float dt);
RcVector rc_actor_get_position(RcActor *a);
RcVector rc_actor_get_direction(RcActor *a);
uint32_t rc_actor_get_texture(RcActor *a);
void rc_actor_destroy(RcActor *a);

#endif // RAYCASTER_ACTOR_H
