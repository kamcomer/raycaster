#ifndef SCENE_H
#define SCENE_H

#include "window_ctx.h"
#include "actor.h"
#include "map.h"
#include "player.h"
#include "sprite.h"
#include "texture.h"

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
} Scene;

Scene *create_scene(WindowCtx *window_ctx, char *map_path);
void free_scene(Scene *scene);

void render_scene(Scene *scene, void (*render)(Scene*));
void render_2d_scene(Scene *scene);
void render_fp_scene(Scene *scene);
void renderer_sprites(Scene *scene);
void render_2d_map(Scene scene);
void render_2d_player(Scene *scene);
void render_player_plane(Scene *scene);
void render_player_view_rays(Scene *scene);
void render_actor_body(Scene *scene);
void render_actor_vel_dir(Scene *scene);
void render_actor_view_dir(Scene *scene);
void render_actor_view_rays(Scene *scene);
void render_walls(Scene *scene);
void render_floor_and_ceil(Scene *scene);

#endif
