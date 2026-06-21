#include "scene.h"
#include "render_fp.h"
#include "hud.h"
#include "renderer.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
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
    return NULL;
  }

  scene->window_ctx = window_ctx;

  Map map = load_map(map_path);
  if (map.walls == NULL || map.ceil == NULL || map.floor == NULL)
  {
    fprintf(stderr, "Failed to load map from file\n");
    free(scene);
    return NULL;
  }

  TextureData *textures = create_textures();
  if (textures == NULL)
  {
    fprintf(stderr, "Failed to load textures\n");
    free_map(map);
    free(scene);
    return NULL;
  }

  SceneStaticSprites static_sprites;
  static_sprites.sprites = malloc(sizeof(StaticSprite) * 10);
  if (static_sprites.sprites == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for static sprits\n");
    free_texture(textures);
    free_map(map);
    free(scene);
    return NULL;
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
    free_texture(textures);
    free_map(map);
    free(scene);
    return NULL;
  }
  static_sprites.sprite_dist = malloc(sizeof(double) * static_sprites.num_sprites);
  if (static_sprites.sprite_dist == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for sprite dist\n");
    free(static_sprites.sprite_order);
    free(static_sprites.sprites);
    free_texture(textures);
    free_map(map);
    free(scene);
    return NULL;
  }

  scene->map = map;
  scene->static_sprites = static_sprites;
  scene->textures = textures;

  scene->player = create_player(window_ctx);
  if (scene->player.actor == NULL)
  {
    fprintf(stderr, "Failed to create player\n");
    free(static_sprites.sprite_dist);
    free(static_sprites.sprite_order);
    free(static_sprites.sprites);
    free_texture(textures);
    free_map(map);
    free(scene);
    return NULL;
  }

  int w = window_ctx->window_config->width;
  int h = window_ctx->window_config->height;
  scene->wall_texture = create_texture(
      window_ctx->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);
  scene->floor_ceil_texture = create_texture(
      window_ctx->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);
  scene->sprite_texture = create_texture(
      window_ctx->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);

  if (!scene->wall_texture || !scene->floor_ceil_texture || !scene->sprite_texture)
  {
    fprintf(stderr, "Failed to create render textures\n");
    free_player(&scene->player);
    free(static_sprites.sprite_dist);
    free(static_sprites.sprite_order);
    free(static_sprites.sprites);
    free_texture(textures);
    free_map(map);
    free(scene);
    return NULL;
  }

  set_texture_blend_mode(scene->sprite_texture, SDL_BLENDMODE_BLEND);
  set_texture_blend_mode(scene->wall_texture, SDL_BLENDMODE_BLEND);

  scene->use_gpu_renderer = false;
  printf("Using CPU renderer (GPU not available)\n");

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
  if (scene->gpu_renderer)
  {
    rc_gpu_renderer_destroy(scene->gpu_renderer);
  }
  free_player(&scene->player);
  free_map(scene->map);
  free_texture(scene->textures);
  free(scene->static_sprites.sprites);
  free(scene->static_sprites.sprite_order);
  free(scene->static_sprites.sprite_dist);
  destroy_texture(scene->wall_texture);
  destroy_texture(scene->floor_ceil_texture);
  destroy_texture(scene->sprite_texture);
  free(scene);
}
