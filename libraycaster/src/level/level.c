#include "internal/level/level_int.h"

int rc_level_get_width(RcLevel *w) { return w->vtbl->width(w); }

int rc_level_get_height(RcLevel *w) { return w->vtbl->height(w); }

int rc_level_get_wall(RcLevel *w, int x, int y) { return w->vtbl->wall(w, x, y); }

int rc_level_get_floor(RcLevel *w, int x, int y) { return w->vtbl->floor(w, x, y); }

int rc_level_get_ceil(RcLevel *w, int x, int y) { return w->vtbl->ceil(w, x, y); }

int rc_level_get_unit_size(RcLevel *w) { return w->vtbl->unit_size(w); }

void rc_level_get_sprites(RcLevel *w, RcSprite **out, int *count)
{
  w->vtbl->sprites(w, out, count);
}

void rc_level_update(RcLevel *w, float dt) { w->vtbl->update(w, dt); }

void rc_level_destroy(RcLevel *w)
{
  if (w && w->vtbl && w->vtbl->destroy)
    w->vtbl->destroy(w);
}
