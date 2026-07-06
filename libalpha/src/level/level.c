#include "internal/i_alpha.h"

uint32_t a_level_get_height(ALevel *level) { return level->vtbl->height(level); }

uint32_t a_level_get_wall(ALevel *level, int x, int y) { return level->vtbl->wall(level, x, y); }

uint32_t a_level_get_floor(ALevel *level, int x, int y) { return level->vtbl->floor(level, x, y); }

uint32_t a_level_get_ceil(ALevel *level, int x, int y) { return level->vtbl->ceil(level, x, y); }

uint32_t a_level_get_unit_size(ALevel *level) { return level->vtbl->unit_size(level); }

StringArray *a_level_get_texture_paths(ALevel *level) { return level->vtbl->texture_paths(level); }

void a_level_get_sprites(ALevel *level, ASprite **out, uint *count)
{
  level->vtbl->sprites(level, out, count);
}

void a_level_update(ALevel *level, float delta_t) { level->vtbl->update(level, delta_t); }

void a_level_destroy(ALevel *level)
{
  if (level && level->vtbl && level->vtbl->destroy) {
    level->vtbl->destroy(level);
  }
}
