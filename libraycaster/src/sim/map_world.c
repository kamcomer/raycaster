#include "raycaster/world.h"

#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_MAP_UNIT_SIZE 20

typedef struct
{
    uint8_t **walls;
    uint8_t **ceil;
    uint8_t **floor;
    int unit_size;
    int width;
    int height;
    RcSprite *sprites;
    int sprite_count;
} MapWorldData;

static int mapworld_width(RcWorld *w)
{
    MapWorldData *data = (MapWorldData *)w->impl;
    return data->width;
}

static int mapworld_height(RcWorld *w)
{
    MapWorldData *data = (MapWorldData *)w->impl;
    return data->height;
}

static int mapworld_wall(RcWorld *w, int x, int y)
{
    MapWorldData *data = (MapWorldData *)w->impl;
    if (x < 0 || x >= data->width || y < 0 || y >= data->height)
        return 0;
    return data->walls[y][x];
}

static int mapworld_floor(RcWorld *w, int x, int y)
{
    MapWorldData *data = (MapWorldData *)w->impl;
    if (x < 0 || x >= data->width || y < 0 || y >= data->height)
        return 0;
    return data->floor[y][x];
}

static int mapworld_ceil(RcWorld *w, int x, int y)
{
    MapWorldData *data = (MapWorldData *)w->impl;
    if (x < 0 || x >= data->width || y < 0 || y >= data->height)
        return 0;
    return data->ceil[y][x];
}

static int mapworld_unit_size(RcWorld *w)
{
    (void)w;
    return DEFAULT_MAP_UNIT_SIZE;
}

static void mapworld_sprites(RcWorld *w, RcSprite **out, int *count)
{
    MapWorldData *data = (MapWorldData *)w->impl;
    *out = data->sprites;
    *count = data->sprite_count;
}

static void mapworld_update(RcWorld *w, float dt)
{
    (void)w;
    (void)dt;
}

static void mapworld_destroy(RcWorld *w)
{
    if (!w)
        return;

    MapWorldData *data = (MapWorldData *)w->impl;
    if (!data)
        return;

    for (int i = 0; i < data->height; i++)
    {
        free(data->walls[i]);
        free(data->ceil[i]);
        free(data->floor[i]);
    }
    free(data->walls);
    free(data->ceil);
    free(data->floor);
    free(data->sprites);
    free(data);
    free(w);
}

static RcWorldVtbl mapworld_vtbl = {
    .width = mapworld_width,
    .height = mapworld_height,
    .wall = mapworld_wall,
    .floor = mapworld_floor,
    .ceil = mapworld_ceil,
    .unit_size = mapworld_unit_size,
    .sprites = mapworld_sprites,
    .update = mapworld_update,
    .destroy = mapworld_destroy,
};

static int load_map_grid(const char *filename, MapWorldData *map)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Failed to open map file: %s\n", filename);
        return -1;
    }

    fscanf(file, "%d %d", &map->width, &map->height);

    map->walls = malloc(map->height * sizeof(uint8_t *));
    map->ceil = malloc(map->height * sizeof(uint8_t *));
    map->floor = malloc(map->height * sizeof(uint8_t *));

    if (!map->walls || !map->ceil || !map->floor)
    {
        fprintf(stderr, "Failed to allocate memory for map\n");
        if (map->walls) free(map->walls);
        if (map->ceil) free(map->ceil);
        if (map->floor) free(map->floor);
        fclose(file);
        return -1;
    }

    for (int i = 0; i < map->height; i++)
    {
        map->walls[i] = malloc(map->width * sizeof(uint8_t));
        for (int j = 0; j < map->width; j++)
        {
            if (fscanf(file, "%hhu", &map->walls[i][j]) != 1)
            {
                fprintf(stderr, "Failed to read walls[%d][%d]\n", i, j);
                fclose(file);
                return -1;
            }
        }
    }

    for (int i = 0; i < map->height; i++)
    {
        map->ceil[i] = malloc(map->width * sizeof(uint8_t));
        for (int j = 0; j < map->width; j++)
        {
            if (fscanf(file, "%hhu", &map->ceil[i][j]) != 1)
            {
                fprintf(stderr, "Failed to read ceil[%d][%d]\n", i, j);
                fclose(file);
                return -1;
            }
        }
    }

    for (int i = 0; i < map->height; i++)
    {
        map->floor[i] = malloc(map->width * sizeof(uint8_t));
        for (int j = 0; j < map->width; j++)
        {
            if (fscanf(file, "%hhu", &map->floor[i][j]) != 1)
            {
                fprintf(stderr, "Failed to read floor[%d][%d]\n", i, j);
                fclose(file);
                return -1;
            }
        }
    }

    fclose(file);
    return 0;
}

RcWorld *rc_world_load_from_file(const char *map_path)
{
    MapWorldData *data = calloc(1, sizeof(MapWorldData));
    if (!data)
    {
        fprintf(stderr, "Failed to allocate MapWorldData\n");
        return NULL;
    }

    data->unit_size = DEFAULT_MAP_UNIT_SIZE;
    data->sprite_count = 3;
    data->sprites = malloc(sizeof(RcSprite) * data->sprite_count);
    
    data->sprites[0].pos.x = 10.5;
    data->sprites[0].pos.y = 10.5;
    data->sprites[0].texture_id = 1;
    data->sprites[0].is_dynamic = false;
    
    data->sprites[1].pos.x = 15.5;
    data->sprites[1].pos.y = 15.5;
    data->sprites[1].texture_id = 2;
    data->sprites[1].is_dynamic = false;
    
    data->sprites[2].pos.x = 5.5;
    data->sprites[2].pos.y = 5.5;
    data->sprites[2].texture_id = 3;
    data->sprites[2].is_dynamic = false;

    if (load_map_grid(map_path, data) != 0)
    {
        free(data);
        return NULL;
    }

    RcWorld *world = malloc(sizeof(RcWorld));
    if (!world)
    {
        free(data);
        return NULL;
    }

    world->vtbl = &mapworld_vtbl;
    world->impl = data;

    return world;
}

RcWorld *rc_world_create_empty(int width, int height)
{
    MapWorldData *data = calloc(1, sizeof(MapWorldData));
    if (!data)
        return NULL;

    data->width = width;
    data->height = height;
    data->unit_size = DEFAULT_MAP_UNIT_SIZE;

    data->walls = calloc(height, sizeof(uint8_t *));
    data->ceil = calloc(height, sizeof(uint8_t *));
    data->floor = calloc(height, sizeof(uint8_t *));

    for (int i = 0; i < height; i++)
    {
        data->walls[i] = calloc(width, sizeof(uint8_t));
        data->ceil[i] = calloc(width, sizeof(uint8_t));
        data->floor[i] = calloc(width, sizeof(uint8_t));
    }

    RcWorld *world = malloc(sizeof(RcWorld));
    if (!world)
    {
        free(data);
        return NULL;
    }

    world->vtbl = &mapworld_vtbl;
    world->impl = data;
    return world;
}

int rc_world_get_width(RcWorld *w)
{
    return w->vtbl->width(w);
}

int rc_world_get_height(RcWorld *w)
{
    return w->vtbl->height(w);
}

int rc_world_get_wall(RcWorld *w, int x, int y)
{
    return w->vtbl->wall(w, x, y);
}

int rc_world_get_floor(RcWorld *w, int x, int y)
{
    return w->vtbl->floor(w, x, y);
}

int rc_world_get_ceil(RcWorld *w, int x, int y)
{
    return w->vtbl->ceil(w, x, y);
}

int rc_world_get_unit_size(RcWorld *w)
{
    return w->vtbl->unit_size(w);
}

void rc_world_get_sprites(RcWorld *w, RcSprite **out, int *count)
{
    w->vtbl->sprites(w, out, count);
}

void rc_world_update(RcWorld *w, float dt)
{
    w->vtbl->update(w, dt);
}

void rc_world_destroy(RcWorld *w)
{
    if (w && w->vtbl && w->vtbl->destroy)
    {
        w->vtbl->destroy(w);
    }
}
