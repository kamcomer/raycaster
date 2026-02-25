#ifndef RAYCASTER_WORLD_H
#define RAYCASTER_WORLD_H

#include "types.h"
#include "camera.h"

typedef struct RcWorld RcWorld;

typedef struct RcSprite
{
    RcVector pos;
    int texture_id;
    bool is_dynamic;
} RcSprite;

typedef struct RcWorldVtbl
{
    int (*width)(RcWorld *w);
    int (*height)(RcWorld *w);
    int (*wall)(RcWorld *w, int x, int y);
    int (*floor)(RcWorld *w, int x, int y);
    int (*ceil)(RcWorld *w, int x, int y);
    int (*unit_size)(RcWorld *w);
    void (*sprites)(RcWorld *w, RcSprite **out, int *count);
    void (*update)(RcWorld *w, float dt);
    void (*destroy)(RcWorld *w);
} RcWorldVtbl;

struct RcWorld
{
    const RcWorldVtbl *vtbl;
    void *impl;
};

int rc_world_get_width(RcWorld *w);
int rc_world_get_height(RcWorld *w);
int rc_world_get_wall(RcWorld *w, int x, int y);
int rc_world_get_floor(RcWorld *w, int x, int y);
int rc_world_get_ceil(RcWorld *w, int x, int y);
int rc_world_get_unit_size(RcWorld *w);
void rc_world_get_sprites(RcWorld *w, RcSprite **out, int *count);
void rc_world_update(RcWorld *w, float dt);
void rc_world_destroy(RcWorld *w);

#endif // RAYCASTER_WORLD_H
