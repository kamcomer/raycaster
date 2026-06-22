#include "internal/level/map_level_int.h"
#include "internal/util/general.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_grid_rows(FILE *file, uint8_t **grid, int width, int height)
{
  for (int i = 0; i < height; i++) {
    grid[i] = malloc(width * sizeof(uint8_t));
    if (!grid[i])
      return -1;
    for (int j = 0; j < width; j++) {
      if (fscanf(file, "%hhu", &grid[i][j]) != 1)
        return -1;
    }
  }
  return 0;
}

static int allocate_grids(MapLevelData *map)
{
  map->walls = malloc(map->height * sizeof(uint8_t *));
  map->ceil = malloc(map->height * sizeof(uint8_t *));
  map->floor = malloc(map->height * sizeof(uint8_t *));
  if (!map->walls || !map->ceil || !map->floor) {
    free(map->walls);
    free(map->ceil);
    free(map->floor);
    map->walls = NULL;
    map->ceil = NULL;
    map->floor = NULL;
    return -1;
  }
  return 0;
}

static int parse_grid_rows_from_buf(FILE *file, uint8_t **grid, const char *buf, int width,
                                    int height)
{
  for (int j = 0; j < width; j++) {
    char *end;
    long v = strtol(buf, &end, 10);
    if (end == buf)
      return -1;
    grid[0][j] = (uint8_t)v;
    buf = (const char *)end;
  }
  for (int i = 1; i < height; i++) {
    grid[i] = malloc(width * sizeof(uint8_t));
    if (!grid[i])
      return -1;
    for (int j = 0; j < width; j++) {
      if (fscanf(file, "%hhu", &grid[i][j]) != 1)
        return -1;
    }
  }
  return 0;
}

static void destroy_data(MapLevelData *data)
{
  if (!data)
    return;
  for (int i = 0; i < data->height; i++) {
    free(data->walls ? data->walls[i] : NULL);
    free(data->ceil ? data->ceil[i] : NULL);
    free(data->floor ? data->floor[i] : NULL);
  }
  free(data->walls);
  free(data->ceil);
  free(data->floor);
  free(data->sprites);
  for (int i = 0; i < data->texture_count; i++)
    free(data->texture_paths[i]);
  for (int i = 0; i < data->sprite_type_count; i++)
    free(data->sprite_type_paths[i]);
  free(data);
}

RcLevel *rc_level_load_from_file(const char *file_path)
{
  MapLevelData *data = calloc(1, sizeof(MapLevelData));
  if (!data)
    return NULL;

  FILE *file = fopen(file_path, "r");
  if (!file) {
    free(data);
    return NULL;
  }

  char buf[1024];
  MapSection section = MAP_SECTION_NONE;
  int sprite_capacity = 4;

  while (fgets(buf, sizeof(buf), file)) {
    trim_line(buf);
    if (buf[0] == '#' || buf[0] == '\0')
      continue;

    if (strcmp(buf, "[MAP]") == 0) {
      section = MAP_SECTION_MAP;
      continue;
    }
    if (strcmp(buf, "[CEIL]") == 0) {
      section = MAP_SECTION_CEIL;
      continue;
    }
    if (strcmp(buf, "[FLOOR]") == 0) {
      section = MAP_SECTION_FLOOR;
      continue;
    }
    if (strcmp(buf, "[TEXTURES]") == 0) {
      section = MAP_SECTION_TEXTURES;
      continue;
    }
    if (strcmp(buf, "[SPRITE_TYPES]") == 0) {
      section = MAP_SECTION_SPRITE_TYPES;
      continue;
    }
    if (strcmp(buf, "[SPRITES]") == 0) {
      section = MAP_SECTION_SPRITES;
      data->sprites = malloc(sprite_capacity * sizeof(RcSprite));
      data->sprite_count = 0;
      continue;
    }

    switch (section) {
    case MAP_SECTION_MAP:
      if (sscanf(buf, "%d %d", &data->width, &data->height) != 2) {
        fclose(file);
        destroy_data(data);
        return NULL;
      }
      if (allocate_grids(data) != 0) {
        fclose(file);
        destroy_data(data);
        return NULL;
      }
      if (parse_grid_rows(file, data->walls, data->width, data->height) != 0) {
        fclose(file);
        destroy_data(data);
        return NULL;
      }
      section = MAP_SECTION_NONE;
      break;
    case MAP_SECTION_CEIL:
      data->ceil = malloc(data->height * sizeof(uint8_t *));
      if (!data->ceil) {
        fclose(file);
        destroy_data(data);
        return NULL;
      }
      data->ceil[0] = malloc(data->width * sizeof(uint8_t));
      if (!data->ceil[0]) {
        fclose(file);
        destroy_data(data);
        return NULL;
      }
      if (parse_grid_rows_from_buf(file, data->ceil, buf, data->width, data->height) != 0) {
        fclose(file);
        destroy_data(data);
        return NULL;
      }
      section = MAP_SECTION_NONE;
      break;
    case MAP_SECTION_FLOOR:
      data->floor = malloc(data->height * sizeof(uint8_t *));
      if (!data->floor) {
        fclose(file);
        destroy_data(data);
        return NULL;
      }
      data->floor[0] = malloc(data->width * sizeof(uint8_t));
      if (!data->floor[0]) {

        fclose(file);
        destroy_data(data);
        return NULL;
      }
      if (parse_grid_rows_from_buf(file, data->floor, buf, data->width, data->height) != 0) {
        fclose(file);
        destroy_data(data);
        return NULL;
      }
      section = MAP_SECTION_NONE;
      break;
    case MAP_SECTION_TEXTURES: {
      if (data->texture_count < MAX_TEXTURE_PATHS) {
        char *path = buf;
        char *colon = strchr(buf, ':');
        if (colon != NULL && colon[1] == ' ')
          path = colon + 2;
        data->texture_paths[data->texture_count] = malloc(strlen(path) + 1);
        if (data->texture_paths[data->texture_count]) {
          strcpy(data->texture_paths[data->texture_count], path);
          data->texture_count++;
        }
      }
      break;
    }
    case MAP_SECTION_SPRITE_TYPES: {
      if (data->sprite_type_count < MAX_SPRITE_TYPES) {
        char *colon = strchr(buf, ':');
        if (colon != NULL && colon[1] == ' ') {
          int key_len = colon - buf;
          if (key_len >= MAX_SPRITE_TYPE_KEY_LEN)
            key_len = MAX_SPRITE_TYPE_KEY_LEN - 1;
          strncpy(data->sprite_type_keys[data->sprite_type_count], buf, key_len);
          data->sprite_type_keys[data->sprite_type_count][key_len] = '\0';
          const char *path = colon + 2;
          data->sprite_type_paths[data->sprite_type_count] = malloc(strlen(path) + 1);
          if (data->sprite_type_paths[data->sprite_type_count]) {
            strcpy(data->sprite_type_paths[data->sprite_type_count], path);
            data->sprite_type_count++;
          }
        }
      }
      break;
    }
    case MAP_SECTION_SPRITES: {
      RcSprite s;
      char key[MAX_SPRITE_TYPE_KEY_LEN];
      if (sscanf(buf, "%lf %lf %63s", &s.pos.x, &s.pos.y, key) == 3) {
        s.texture_id = -1;
        for (int i = 0; i < data->sprite_type_count; i++) {
          if (strcmp(data->sprite_type_keys[i], key) == 0) {
            s.texture_id = i;
            break;
          }
        }
        if (s.texture_id < 0)
          s.texture_id = 0;
        s.is_dynamic = false;
        s.pos.mag = 0;
        s.pos.angle = 0;
        if (data->sprite_count >= sprite_capacity) {
          sprite_capacity *= 2;
          data->sprites = realloc(data->sprites, sprite_capacity * sizeof(RcSprite));
        }
        data->sprites[data->sprite_count] = s;
        data->sprite_count++;
      }
      break;
    }
    default:
      break;
    }
  }

  fclose(file);
  data->unit_size = DEFAULT_MAP_UNIT_SIZE;
  RcLevel *world = malloc(sizeof(RcLevel));
  if (!world) {
    destroy_data(data);
    return NULL;
  }
  world->vtbl = &maplevel_vtbl;
  world->impl = data;
  return world;
}

RcLevel *rc_level_create_empty(int width, int height)
{
  MapLevelData *data = calloc(1, sizeof(MapLevelData));
  if (!data)
    return NULL;

  data->width = width;
  data->height = height;
  data->unit_size = DEFAULT_MAP_UNIT_SIZE;

  data->walls = calloc(height, sizeof(uint8_t *));
  data->ceil = calloc(height, sizeof(uint8_t *));
  data->floor = calloc(height, sizeof(uint8_t *));
  if (!data->walls || !data->ceil || !data->floor) {
    free(data->walls);
    free(data->ceil);
    free(data->floor);
    free(data);
    return NULL;
  }

  for (int i = 0; i < height; i++) {
    data->walls[i] = calloc(width, sizeof(uint8_t));
    data->ceil[i] = calloc(width, sizeof(uint8_t));
    data->floor[i] = calloc(width, sizeof(uint8_t));
    if (!data->walls[i] || !data->ceil[i] || !data->floor[i]) {
      destroy_data(data);
      return NULL;
    }
  }

  RcLevel *world = malloc(sizeof(RcLevel));
  if (!world) {
    destroy_data(data);
    return NULL;
  }
  world->vtbl = &maplevel_vtbl;
  world->impl = data;
  return world;
}

// RcLevel Vtbl implementation
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
  for (int i = 0; i < data->sprite_type_count; i++)
    free(data->sprite_type_paths[i]);
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
