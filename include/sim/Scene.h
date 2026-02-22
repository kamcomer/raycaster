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
void set_current_scene(Scene *scene);
Scene *get_current_scene(void);
void free_scene(Scene *scene);

void render_scene(void (*render)(void));
void render_2d_scene(void);
void render_fp_scene(void);
void renderer_sprites(void);
void render_2d_map(Scene scene);
void render_2d_player(void);
void render_player_plane(Player player);
void render_player_view_rays(Player player);
void render_actor_body(Actor actor);
void render_actor_vel_dir(Actor actor);
void render_actor_view_dir(Actor actor);
void render_actor_view_rays(Actor actor);
void render_walls(void);
void render_floor_and_ceil(void);

#endif
