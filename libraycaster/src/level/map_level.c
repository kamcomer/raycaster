#include "level/map_level_int.h"
#include "util/general.h"

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
  free(data);
}

RcLevel *rc_level_load_from_file(const char *map_path)
{
  MapLevelData *data = calloc(1, sizeof(MapLevelData));
  if (!data)
    return NULL;

  FILE *file = fopen(map_path, "r");
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
    case MAP_SECTION_TEXTURES:
      if (data->texture_count < MAX_TEXTURE_PATHS) {
        data->texture_paths[data->texture_count] = malloc(strlen(buf) + 1);
        if (data->texture_paths[data->texture_count]) {
          strcpy(data->texture_paths[data->texture_count], buf);
          data->texture_count++;
        }
      }
      break;
    case MAP_SECTION_SPRITES: {
      RcSprite s;
      if (sscanf(buf, "%lf %lf %d", &s.pos.x, &s.pos.y, &s.texture_id) == 3) {
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
