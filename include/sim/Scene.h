#ifndef SCENE_H
#define SCENE_H

#include "window_ctx.h"
#include "actor.h"
#include "map.h"
#include "player.h"
#include "sprite.h"
#include "texture.h"

struct Texture;
typedef struct Texture Texture;

typedef struct
{
  Actor *actors;
  int count;
  int list_size;
} ActorList;

typedef struct
{
  int num_sprites;
  int *sprite_order;
  double *sprite_dist;
  StaticSprite *sprites;
} SceneStaticSprites;

typedef struct
{
  int num_sprites;
  int *sprite_order;
  double *sprite_dist;
  DynamicSprite *sprites;
} SceneDynamicSprites;

typedef struct
{
  Map map;
  Player player;
  ActorList *actor_list;
  SceneStaticSprites static_sprites;
  SceneDynamicSprites dynamic_sprites;
  TextureData *textures;
  WindowCtx *window_ctx;
  Texture *wall_texture;
  Texture *floor_ceil_texture;
  Texture *sprite_texture;
} Scene;

Scene *create_scene(WindowCtx *window_ctx, char *map_path);
void free_scene(Scene *scene);

void render_scene(Scene *scene, void (*render)(Scene*));

#endif
