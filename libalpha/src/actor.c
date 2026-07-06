
#include "internal/i_alpha.h"
#include <stdlib.h>

void a_actor_update(AActor *a, ALevel *world, AInput *input, float dt)
{
  if (a && a->vtbl && a->vtbl->update)
    a->vtbl->update(a, world, input, dt);
}

AVector a_actor_get_position(AActor *a)
{
  if (a && a->vtbl && a->vtbl->get_position)
    return a->vtbl->get_position(a);
  AVector v = {0, 0, 0, 0};
  return v;
}

AVector a_actor_get_direction(AActor *a)
{
  if (a && a->vtbl && a->vtbl->get_direction)
    return a->vtbl->get_direction(a);
  AVector v = {0, 0, 0, 0};
  return v;
}

uint32_t a_actor_get_texture(AActor *a)
{
  if (a && a->vtbl && a->vtbl->get_texture)
    return a->vtbl->get_texture(a);
  return 0;
}

void a_actor_destory(AActor *a)
{
  if (a && a->vtbl && a->vtbl->destroy)
    a->vtbl->destroy(a);
}
