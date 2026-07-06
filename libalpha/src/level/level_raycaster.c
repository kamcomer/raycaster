#include "internal/i_alpha.h"
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

static void destroy_data(LevelRaycasterMapData *data)
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
  string_array_destroy(&data->tex_paths);

  for (uint32_t i = 0; i < data->sprite_types.len; i++)
    free(data->sprite_types.items[i].path);
  free(data->sprite_types.items);

  free(data->sprites.items);
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

static int parse_texture_section(char *buf, StringArray *paths)
{
  const char *path = buf;
  char *colon = strchr(buf, ':');
  if (colon != NULL && colon[1] == ' ')
    path = colon + 2;

  return string_array_push(paths, path);
}

static int parse_sprite_type_section(char *buf, ALevelRaycasterSpriteTypes *sprite_types)
{
  char *colon = strchr(buf, ':');
  if (!colon)
    return -1;

  if (colon[1] == ' ') {
    colon += 2;
  }

  char path[1024];
  int frame_count = 1;
  float frame_delay = 0.0f;
  int n = sscanf((const char *)colon, "%1023s %d %f", path, &frame_count, &frame_delay);
  if (n >= 1) {
    if (frame_count < 1)
      frame_count = 1;

    if (sprite_types->len == sprite_types->capacity) {
      sprite_types->capacity *= 2;
      sprite_types->items = realloc(sprite_types->items,
                                    sprite_types->capacity * sizeof(ALevelRaycasterSpriteTypeDef));
      if (!sprite_types->items)
        return -1;
    }
  }

  sprite_types->items[sprite_types->len].path = (char *)malloc(strlen(path) + 1);
  if (!sprite_types->items[sprite_types->len].path)
    return -1;

  strcpy(sprite_types->items[sprite_types->len].path, path);
  sprite_types->items[sprite_types->len].frame_count = frame_count;
  sprite_types->items[sprite_types->len].frame_delay = frame_delay;
  sprite_types->len++;
  return 0;
}

static int parse_sprites_section(char *buf, ASpriteArray *sprites,
                                 ALevelRaycasterSpriteTypes *sprite_types)
{
  ASprite s;
  uint32_t type_id;
  if (sscanf(buf, "%lf %lf %d", &s.pos.x, &s.pos.y, &type_id) != 3) {
    return -1;
  }

  if (type_id < 0)
    type_id = 0;

  if (type_id >= sprite_types->len && sprite_types->len > 0) {
    return -1;
  }

  s.texture_id = type_id;
  s.is_dynamic = false;
  s.pos.mag = 0;
  s.pos.angle = 0;

  if (sprites->len >= sprites->capacity) {
    sprites->capacity *= 2;
    sprites->items = realloc(sprites->items, sprites->capacity * sizeof(ASprite));
  }
  sprites->items[sprites->len] = s;
  sprites->len++;
  return 0;
}

// RcLevel Vtbl implementation
static uint maplevel_width(ALevel *w)
{
  LevelRaycasterMapData *data = (LevelRaycasterMapData *)w->impl;
  return data->width;
}

static uint maplevel_height(ALevel *w)
{
  LevelRaycasterMapData *data = (LevelRaycasterMapData *)w->impl;
  return data->height;
}

static uint maplevel_wall(ALevel *w, int x, int y)
{
  LevelRaycasterMapData *data = (LevelRaycasterMapData *)w->impl;
  if (x < 0 || (size_t)x >= data->width || y < 0 || (size_t)y >= data->height)
    return 0;
  return data->walls[y][x];
}

static uint maplevel_floor(ALevel *w, int x, int y)
{
  LevelRaycasterMapData *data = (LevelRaycasterMapData *)w->impl;
  if (x < 0 || (size_t)x >= data->width || y < 0 || (size_t)y >= data->height)
    return 0;
  return data->floor[y][x];
}

static uint maplevel_ceil(ALevel *w, int x, int y)
{
  LevelRaycasterMapData *data = (LevelRaycasterMapData *)w->impl;
  if (x < 0 || (size_t)x >= data->width || y < 0 || (size_t)y >= data->height)
    return 0;
  return data->ceil[y][x];
}

static uint maplevel_unit_size(ALevel *w)
{
  (void)w;
  return LEVEL_RAYCASTER_DEFAULT_MAP_UNIT_SIZE;
}

static void maplevel_sprites(ALevel *w, ASprite **out, uint *count)
{
  LevelRaycasterMapData *data = (LevelRaycasterMapData *)w->impl;
  *out = data->sprites.items;
  *count = data->sprites.len;
}

static StringArray *maplevel_texture_paths(ALevel *w)
{
  LevelRaycasterMapData *data = (LevelRaycasterMapData *)w->impl;
  return &data->tex_paths;
}

static void maplevel_update(ALevel *w, float dt)
{
  (void)w;
  (void)dt;
}

static void maplevel_destroy(ALevel *w)
{
  if (!w)
    return;

  LevelRaycasterMapData *data = (LevelRaycasterMapData *)w->impl;
  if (!data)
    return;

  destroy_data(data);
  free(w);
}

ALevelVtbl maplevel_vtbl = {
    .width = maplevel_width,
    .height = maplevel_height,
    .wall = maplevel_wall,
    .floor = maplevel_floor,
    .ceil = maplevel_ceil,
    .unit_size = maplevel_unit_size,
    .sprites = maplevel_sprites,
    .texture_paths = maplevel_texture_paths,
    .update = maplevel_update,
    .destroy = maplevel_destroy,
};

ALevel *a_level_load_from_file(const char *file_path)
{
  LevelRaycasterMapData *data = calloc(1, sizeof(LevelRaycasterMapData));
  if (!data)
    return NULL;

  if (string_array_init(&data->tex_paths, LEVEL_RAYCASTER_DEFAULT_NUM_ASSETS) != 0) {
    destroy_data(data);
    return NULL;
  }

  data->sprite_types.items = (ALevelRaycasterSpriteTypeDef *)malloc(
      LEVEL_RAYCASTER_DEFAULT_NUM_ASSETS * sizeof(ALevelRaycasterSpriteTypeDef));
  if (!data->sprite_types.items) {
    destroy_data(data);
    return NULL;
  }
  data->sprite_types.len = 0;
  data->sprite_types.capacity = LEVEL_RAYCASTER_DEFAULT_NUM_ASSETS;

  data->sprites.items = (ASprite *)malloc(LEVEL_RAYCASTER_DEFAULT_NUM_ASSETS * sizeof(ASprite));
  if (!data->sprites.items) {
    destroy_data(data);
    return NULL;
  }
  data->sprites.len = 0;
  data->sprites.capacity = LEVEL_RAYCASTER_DEFAULT_NUM_ASSETS;

  FILE *file = fopen(file_path, "r");
  if (!file) {
    destroy_data(data);
    return NULL;
  }

  char buf[1024];
  LevelRaycasterFileSection section = MAP_SECTION_NONE;

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
      continue;
    }
    if (strcmp(buf, "[SPRITES]") == 0) {
      section = MAP_SECTION_SPRITES;
      continue;
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
      if (parse_texture_section(buf, &data->tex_paths) != 0) {
        fclose(file);
        destroy_data(data);
        return NULL;
      }
      break;
    }
    case MAP_SECTION_SPRITE_TYPES: {
      if (parse_sprite_type_section(buf, &data->sprite_types) != 0) {
        fclose(file);
        destroy_data(data);
        return NULL;
      }
      break;
    }
    case MAP_SECTION_SPRITES: {
      if (parse_sprites_section(buf, &data->sprites, &data->sprite_types) != 0) {
        fclose(file);
        destroy_data(data);
        return NULL;
      }
      break;
    }
    default:
      break;
    }
  }

  fclose(file);
  data->unit_size = LEVEL_RAYCASTER_DEFAULT_MAP_UNIT_SIZE;
  ALevel *level = malloc(sizeof(ALevel));
  if (!level) {
    destroy_data(data);
    return NULL;
  }
  level->vtbl = &maplevel_vtbl;
  level->impl = data;
  return level;
}

ALevel *a_level_create_empty(uint32_t width, uint32_t height)
{
  LevelRaycasterMapData *data = calloc(1, sizeof(LevelRaycasterMapData));
  if (!data)
    return NULL;

  data->width = width;
  data->height = height;
  data->unit_size = LEVEL_RAYCASTER_DEFAULT_MAP_UNIT_SIZE;

  if (string_array_init(&data->tex_paths, LEVEL_RAYCASTER_DEFAULT_NUM_ASSETS) != 0) {
    free(data);
    return NULL;
  }

  data->walls = calloc(height, sizeof(uint8_t *));
  data->ceil = calloc(height, sizeof(uint8_t *));
  data->floor = calloc(height, sizeof(uint8_t *));
  if (!data->walls || !data->ceil || !data->floor) {
    destroy_data(data);
    return NULL;
  }

  for (size_t i = 0; i < height; i++) {
    data->walls[i] = calloc(width, sizeof(uint8_t));
    data->ceil[i] = calloc(width, sizeof(uint8_t));
    data->floor[i] = calloc(width, sizeof(uint8_t));
    if (!data->walls[i] || !data->ceil[i] || !data->floor[i]) {
      destroy_data(data);
      return NULL;
    }
  }

  ALevel *level = malloc(sizeof(ALevel));
  if (!level) {
    destroy_data(data);
    return NULL;
  }
  level->vtbl = &maplevel_vtbl;
  level->impl = data;
  return level;
}
