#include "internal/level/map_level_int.h"

#include <stdio.h>
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

static int allocate_grid(uint8_t ***grid, int height)
{
  *grid = malloc(height * sizeof(uint8_t *));
  if (!grid) {
    return -1;
  }
  return 0;
}

static void destroy_data(MapLevelData *data)
{
  if (!data)
    return;
  for (size_t i = 0; i < data->height; i++) {
    free(data->walls ? data->walls[i] : NULL);
    free(data->ceil ? data->ceil[i] : NULL);
    free(data->floor ? data->floor[i] : NULL);
  }
  free(data->walls);
  free(data->ceil);
  free(data->floor);
  free(data->sprites);
  string_array_destroy(data->tex_paths);

  for (uint32_t i = 0; i < data->sprite_type_count; i++)
    free(data->sprite_types[i].path);
  free(data->sprite_types);
  free(data);
}

static int parse_map_section(FILE *file, uint8_t ***grid, int width, int height)
{
  if (allocate_grid(grid, height) != 0) {
    return -1;
  }
  if (parse_grid_rows(file, *grid, width, height) != 0) {
    return -1;
  }
  return 0;
}

static int parse_texture_path(char *buf, StringArray *paths)
{
  if (paths->len == paths->max_len) {
    char **old_paths = paths->strs;
    paths = duplicate_string_array(paths, paths->len + 10);

    // Free old strs
    for (size_t i = 0; i < paths->len; i++) {
      free(old_paths[i]);
    }
    free(old_paths);
    if (paths->strs == NULL) {
      return -1;
    }

    paths->max_len += 10;
  }

  const char *path = buf;
  char *colon = strchr(buf, ':');
  if (colon != NULL && colon[1] == ' ')
    path = colon + 2;
  paths->strs[paths->len] = malloc(strlen(path) + 1);
  if (!paths->strs[paths->len])
    return -1;

  strcpy(paths->strs[paths->len], path);
  paths->len++;

  return 0;
}

RcLevel *rc_level_load_from_file(const char *file_path)
{
  MapLevelData *data = calloc(1, sizeof(MapLevelData));
  if (!data)
    return NULL;

  data->tex_paths = string_array_create(DEFAULT_NUM_ASSETS);
  if (!data->tex_paths) {
    destroy_data(data);
    return NULL;
  }

  FILE *file = fopen(file_path, "r");
  if (!file) {
    free(data);
    return NULL;
  }

  char buf[1024];
  LevelFileSection section = MAP_SECTION_NONE;
  int sprite_capacity = 4;

  while (fgets(buf, sizeof(buf), file)) {
    trim_line(buf);
    if (buf[0] == '#' || buf[0] == '\0')
      continue;

    if (strcmp(buf, "[MAP]") == 0) {
      section = MAP_SECTION_MAP;
    }
    if (strcmp(buf, "[CEIL]") == 0) {
      section = MAP_SECTION_CEIL;
    }
    if (strcmp(buf, "[FLOOR]") == 0) {
      section = MAP_SECTION_FLOOR;
    }
    if (strcmp(buf, "[TEXTURES]") == 0) {
      section = MAP_SECTION_TEXTURES;
      continue;
    }
    if (strcmp(buf, "[SPRITE_TYPES]") == 0) {
      section = MAP_SECTION_SPRITE_TYPES;
    }
    if (strcmp(buf, "[SPRITES]") == 0) {
      section = MAP_SECTION_SPRITES;
      data->sprites = malloc(sprite_capacity * sizeof(RcSprite));
      data->sprite_count = 0;
    }

    switch (section) {
    case MAP_SECTION_MAP:
      if (fscanf(file, "%d %d", (int *)&data->width, (int *)&data->height) != 2) {
        fclose(file);
        destroy_data(data);
        return NULL;
      }
      if (parse_map_section(file, &data->walls, data->width, data->height) != 0) {
        fclose(file);
        destroy_data(data);
        return NULL;
      }
      section = MAP_SECTION_NONE;
      break;
    case MAP_SECTION_CEIL:
      if (parse_map_section(file, &data->ceil, data->width, data->height) != 0) {
        fclose(file);
        destroy_data(data);
        return NULL;
      }
      section = MAP_SECTION_NONE;
      break;
    case MAP_SECTION_FLOOR:
      if (parse_map_section(file, &data->floor, data->width, data->height) != 0) {
        fclose(file);
        destroy_data(data);
        return NULL;
      }
      section = MAP_SECTION_NONE;
      break;
    case MAP_SECTION_TEXTURES: {
      if (parse_texture_path(buf, data->tex_paths) != 0) {
        fclose(file);
        destroy_data(data);
        return NULL;
      }
      break;
    }
    case MAP_SECTION_SPRITE_TYPES: {
      char *colon = strchr(buf, ':');
      if (colon != NULL && colon[1] == ' ') {
        const char *rest = colon + 2;
        char path[1024];
        int frame_count = 1;
        float frame_delay = 0.0f;
        int n = sscanf(rest, "%1023s %d %f", path, &frame_count, &frame_delay);
        if (n >= 1) {
          if (frame_count < 1) frame_count = 1;
          uint32_t idx = data->sprite_type_count++;
          SpriteTypeDef *tmp = realloc(data->sprite_types, data->sprite_type_count * sizeof(SpriteTypeDef));
          if (!tmp) break;
          data->sprite_types = tmp;
          data->sprite_types[idx].path = malloc(strlen(path) + 1);
          if (!data->sprite_types[idx].path) break;
          strcpy(data->sprite_types[idx].path, path);
          data->sprite_types[idx].frame_count = frame_count;
          data->sprite_types[idx].frame_delay = frame_delay;
        }
      }
      break;
    }
    case MAP_SECTION_SPRITES: {
      RcSprite s;
      int type_id;
      if (sscanf(buf, "%lf %lf %d", &s.pos.x, &s.pos.y, &type_id) == 3) {
        if (type_id < 0) type_id = 0;
        if (type_id >= data->sprite_type_count && data->sprite_type_count > 0)
          type_id = data->sprite_type_count - 1;
        s.texture_id = type_id;
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

RcLevel *rc_level_create_empty(uint32_t width, uint32_t height)
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
static uint32_t maplevel_width(RcLevel *w)
{
  MapLevelData *data = (MapLevelData *)w->impl;
  return data->width;
}

static uint32_t maplevel_height(RcLevel *w)
{
  MapLevelData *data = (MapLevelData *)w->impl;
  return data->height;
}

static uint32_t maplevel_wall(RcLevel *w, int x, int y)
{
  MapLevelData *data = (MapLevelData *)w->impl;
  if (x < 0 || (size_t)x >= data->width || y < 0 || (size_t)y >= data->height)
    return 0;
  return data->walls[y][x];
}

static uint32_t maplevel_floor(RcLevel *w, int x, int y)
{
  MapLevelData *data = (MapLevelData *)w->impl;
  if (x < 0 || (size_t)x >= data->width || y < 0 || (size_t)y >= data->height)
    return 0;
  return data->floor[y][x];
}

static uint32_t maplevel_ceil(RcLevel *w, int x, int y)
{
  MapLevelData *data = (MapLevelData *)w->impl;
  if (x < 0 || (size_t)x >= data->width || y < 0 || (size_t)y >= data->height)
    return 0;
  return data->ceil[y][x];
}

static uint32_t maplevel_unit_size(RcLevel *w)
{
  (void)w;
  return DEFAULT_MAP_UNIT_SIZE;
}

static void maplevel_sprites(RcLevel *w, RcSprite **out, uint32_t *count)
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

  for (size_t i = 0; i < data->height; i++) {
    free(data->walls[i]);
    free(data->ceil[i]);
    free(data->floor[i]);
  }
  free(data->walls);
  free(data->ceil);
  free(data->floor);
  string_array_destroy(data->tex_paths);

  for (uint32_t i = 0; i < data->sprite_type_count; i++)
    free(data->sprite_types[i].path);
  free(data->sprite_types);
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
