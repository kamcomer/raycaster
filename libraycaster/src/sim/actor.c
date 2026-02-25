#include "raycaster/actor.h"

#include <stdlib.h>

void rc_actor_update(RcActor *a, RcWorld *world, RcInput *input, float dt)
{
    if (a && a->vtbl && a->vtbl->update)
        a->vtbl->update(a, world, input, dt);
}

RcVector rc_actor_get_position(RcActor *a)
{
    if (a && a->vtbl && a->vtbl->get_position)
        return a->vtbl->get_position(a);
    RcVector v = {0, 0, 0, 0};
    return v;
}

RcVector rc_actor_get_direction(RcActor *a)
{
    if (a && a->vtbl && a->vtbl->get_direction)
        return a->vtbl->get_direction(a);
    RcVector v = {0, 0, 0, 0};
    return v;
}

int rc_actor_get_texture(RcActor *a)
{
    if (a && a->vtbl && a->vtbl->get_texture)
        return a->vtbl->get_texture(a);
    return 0;
}

void rc_actor_destroy(RcActor *a)
{
    if (a && a->vtbl && a->vtbl->destroy)
        a->vtbl->destroy(a);
}
