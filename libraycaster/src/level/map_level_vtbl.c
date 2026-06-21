#include "level/map_level_int.h"
#include <stdlib.h>

static int maplevel_width(RcLevel *w)
{
  MapLevelData *data = (MapLevelData *)w->impl;
  return data->width;
}

static int maplevel_height(RcLevel *w)
{
  MapLevelData *data = (MapLevelData *)w->impl;
  return data->height;
}

static int maplevel_wall(RcLevel *w, int x, int y)
{
  MapLevelData *data = (MapLevelData *)w->impl;
  if (x < 0 || x >= data->width || y < 0 || y >= data->height)
    return 0;
  return data->walls[y][x];
}

static int maplevel_floor(RcLevel *w, int x, int y)
{
  MapLevelData *data = (MapLevelData *)w->impl;
  if (x < 0 || x >= data->width || y < 0 || y >= data->height)
    return 0;
  return data->floor[y][x];
}

static int maplevel_ceil(RcLevel *w, int x, int y)
{
  MapLevelData *data = (MapLevelData *)w->impl;
  if (x < 0 || x >= data->width || y < 0 || y >= data->height)
    return 0;
  return data->ceil[y][x];
}

static int maplevel_unit_size(RcLevel *w)
{
  (void)w;
  return DEFAULT_MAP_UNIT_SIZE;
}

static void maplevel_sprites(RcLevel *w, RcSprite **out, int *count)
{
  MapLevelData *data = (MapLevelData *)w->impl;
  *out = data->sprites;
  *count = data->sprite_count;
}

static void maplevel_update(RcLevel *w, float dt)
{
  (void)w;
  (void)dt;
}

static void maplevel_destroy(RcLevel *w)
{
  if (!w)
    return;

  MapLevelData *data = (MapLevelData *)w->impl;
  if (!data)
    return;

  for (int i = 0; i < data->height; i++) {
    free(data->walls[i]);
    free(data->ceil[i]);
    free(data->floor[i]);
  }
  free(data->walls);
  free(data->ceil);
  free(data->floor);
  for (int i = 0; i < data->texture_count; i++)
    free(data->texture_paths[i]);
  free(data->sprites);
  free(data);
  free(w);
}

RcLevelVtbl maplevel_vtbl = {
    .width = maplevel_width,
    .height = maplevel_height,
    .wall = maplevel_wall,
    .floor = maplevel_floor,
    .ceil = maplevel_ceil,
    .unit_size = maplevel_unit_size,
    .sprites = maplevel_sprites,
    .update = maplevel_update,
    .destroy = maplevel_destroy,
};
