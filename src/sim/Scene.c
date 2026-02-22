#include "scene.h"
#include "render_2d.h"
#include "render_fp.h"
#include "hud.h"

#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

Scene *create_scene(WindowCtx *window_ctx, char *map_path)
{
  Scene *scene = malloc(sizeof(Scene));
  if (!scene)
  {
    fprintf(stderr, "Failed to allocate memory for scene\n");
    exit(1);
  }

  scene->window_ctx = window_ctx;

  Map map = load_map(map_path);
  if (map.walls == NULL || map.ceil == NULL || map.floor == NULL)
  {
    fprintf(stderr, "Failed to load map from file\n");
    exit(1);
  }

  TextureData *textures = create_textures();
  if (textures == NULL)
  {
    fprintf(stderr, "Failed to load textures\n");
    exit(1);
  }

  SceneStaticSprites static_sprites;
  static_sprites.sprites = malloc(sizeof(StaticSprite) * 10);
  if (static_sprites.sprites == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for static sprits\n");
    exit(1);
  }
  static_sprites.sprites[0].pos = set_vector(10.5, 10.5);
  static_sprites.sprites[0].texture = 10;
  static_sprites.sprites[1].pos = set_vector(15.5, 15.5);
  static_sprites.sprites[1].texture = 9;
  static_sprites.sprites[2].pos = set_vector(5.5, 5.5);
  static_sprites.sprites[2].texture = 8;
  static_sprites.num_sprites = 3;
  static_sprites.sprite_order = malloc(sizeof(int) * static_sprites.num_sprites);
  if (static_sprites.sprite_order == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for sprite order\n");
    free(static_sprites.sprites);
    exit(1);
  }
  static_sprites.sprite_dist = malloc(sizeof(double) * static_sprites.num_sprites);
  if (static_sprites.sprite_dist == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for sprite dist\n");
    free(static_sprites.sprites);
    free(static_sprites.sprite_order);
    exit(1);
  }

  scene->map = map;
  scene->player = create_player(window_ctx);
  scene->static_sprites = static_sprites;
  scene->textures = textures;

  return scene;
}

void render_scene(Scene *scene, void (*render)(Scene*))
{
  set_render_draw_color(scene->window_ctx->renderer, 0, 0, 0, 255);
  clear_renderer(scene->window_ctx->renderer);
  render(scene);
}

void free_scene(Scene *scene)
{
  free_player(&scene->player);
  free_map(scene->map);
  free(scene);
}
