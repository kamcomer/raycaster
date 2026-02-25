#include "render_fp.h"
#include "hud.h"
#include "renderer.h"
#include "sprite.h"
#include "texture.h"
#include "window_ctx.h"

#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void render_fp_scene(Scene *scene)
{
  render_floor_and_ceil(scene);
  render_walls(scene);
  renderer_sprites(scene);
  render_hud(scene);
}

void renderer_sprites(Scene *scene)
{
  Vector player_pos = scene->player.actor->pos;
  player_pos.x /= DEFAULT_MAP_UNIT_SIZE;
  player_pos.y /= DEFAULT_MAP_UNIT_SIZE;

  Texture *texture = scene->sprite_texture;
  set_texture_blend_mode(texture, SDL_BLENDMODE_BLEND);

  void *pixels;
  int pitch;
  lock_texture(texture, NULL, &pixels, &pitch);
  memset(pixels, 0xFFFFFF00, pitch * scene->window_ctx->window_config->height);
  uint32_t *pixel_data = (uint32_t *)pixels;
  uint32_t color;

  SceneStaticSprites static_sprites = scene->static_sprites;

  for (int sprite_index = 0; sprite_index < static_sprites.num_sprites; sprite_index++)
  {
    static_sprites.sprite_order[sprite_index] = sprite_index;
    static_sprites.sprite_dist[sprite_index] =
        ((player_pos.x - static_sprites.sprites[sprite_index].pos.x) *
             (player_pos.x - static_sprites.sprites[sprite_index].pos.x) +
         (player_pos.y - static_sprites.sprites[sprite_index].pos.y) *
             (player_pos.y - static_sprites.sprites[sprite_index].pos.y));
  }
  sort_sprites(static_sprites.sprite_order, static_sprites.sprite_dist,
               static_sprites.num_sprites);

  for (int i = 0; i < static_sprites.num_sprites; i++)
  {
    Vector rel_sprite_pos = set_vector(
        static_sprites.sprites[static_sprites.sprite_order[i]].pos.x - player_pos.x,
        static_sprites.sprites[static_sprites.sprite_order[i]].pos.y - player_pos.y);

    double inv_det = 1.0 / (scene->player.plane.x * scene->player.actor->dir.y -
                            scene->player.actor->dir.x * scene->player.plane.y);
    Vector transform =
        set_vector(inv_det * (scene->player.actor->dir.y * rel_sprite_pos.x -
                              scene->player.actor->dir.x * rel_sprite_pos.y),
                   inv_det * (-scene->player.plane.y * rel_sprite_pos.x +
                              scene->player.plane.x * rel_sprite_pos.y));
    int sprite_screen_x =
        (int)((scene->window_ctx->window_config->width / 2.0) * (1 + transform.x / transform.y));
    int sprite_height = abs((int)(scene->window_ctx->window_config->height / transform.y));

    int draw_start_y = -sprite_height / 2 + scene->window_ctx->window_config->height / 2;
    if (draw_start_y < 0)
      draw_start_y = 0;

    int draw_end_y = sprite_height / 2 + scene->window_ctx->window_config->height / 2;
    if (draw_end_y >= scene->window_ctx->window_config->height)
      draw_end_y = scene->window_ctx->window_config->height - 1;

    int sprite_width = abs((int)(scene->window_ctx->window_config->height / transform.y));
    int draw_start_x = -sprite_width / 2 + sprite_screen_x;
    if (draw_start_x < 0)
      draw_start_x = 0;

    int draw_end_x = sprite_width / 2 + sprite_screen_x;
    if (draw_end_x >= scene->window_ctx->window_config->width)
      draw_end_x = scene->window_ctx->window_config->width - 1;

    for (int stripe = draw_start_x; stripe < draw_end_x; stripe++)
    {
      int tex_x = (int)(256 * (stripe - (-sprite_width / 2 + sprite_screen_x)) *
                        DEFAULT_TEXTURE_WIDTH / sprite_width) /
                  256;
      if (transform.y > 0 && stripe > 0 && stripe < scene->window_ctx->window_config->width &&
          transform.y < scene->player.intersects[stripe].perp_wall_distance)
      {
        for (int y = draw_start_y; y < draw_end_y; y++)
        {
          int d = (y) * 256 - scene->window_ctx->window_config->height * 128 + sprite_height * 128;
          int tex_y = ((d * DEFAULT_TEXTURE_HEIGHT) / sprite_height) / 256;
          color = scene
                      ->textures[static_sprites.sprites[static_sprites.sprite_order[i]]
                                     .texture]
                      .pixels[DEFAULT_TEXTURE_WIDTH * tex_y + tex_x];

          if ((color & 0xFF) == 0xFF)
            pixel_data[(y * (pitch / 4)) + stripe] = color;
        }
      }
    }
  }
  unlock_texture(texture);
  render_copy(scene->window_ctx->renderer, texture, NULL, NULL);
}

void render_floor_and_ceil(Scene *scene)
{
  Texture *texture = scene->floor_ceil_texture;
  void *pixels;
  int pitch;
  lock_texture(texture, NULL, &pixels, &pitch);
  memset(pixels, 0xFFFFFF00, pitch * scene->window_ctx->window_config->height);
  uint32_t *pixel_data = (uint32_t *)pixels;
  uint32_t color;

  int w, h;
  get_window_size_in_pixels(scene->window_ctx->window, &w, &h);
  for (int y = h / 2; y < h; ++y)
  {
    float rayDirX0 = scene->player.actor->dir.x - scene->player.plane.x;
    float rayDirY0 = scene->player.actor->dir.y - scene->player.plane.y;
    float rayDirX1 = scene->player.actor->dir.x + scene->player.plane.x;
    float rayDirY1 = scene->player.actor->dir.y + scene->player.plane.y;

    int p = y - h / 2;
    if (p == 0)
      p = 1;
    if (p < 0)
      p = 1;

    float posZ = 0.5 * h;

    float rowDistance = posZ / p;
    if (rowDistance == 0)
      rowDistance = 0.1;

    float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / w;
    float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / w;

    float floorX =
        scene->player.actor->pos.x / DEFAULT_MAP_UNIT_SIZE + rowDistance * rayDirX0;
    float floorY =
        scene->player.actor->pos.y / DEFAULT_MAP_UNIT_SIZE + rowDistance * rayDirY0;

    for (int x = 0; x < w; x++)
    {
      int cellX = (int)(floorX);
      int cellY = (int)(floorY);

      int tx = (int)(DEFAULT_TEXTURE_WIDTH * (floorX - cellX)) % DEFAULT_TEXTURE_WIDTH;
      int ty = (int)(DEFAULT_TEXTURE_HEIGHT * (floorY - cellY)) % DEFAULT_TEXTURE_HEIGHT;

      floorX += floorStepX;
      floorY += floorStepY;

      if (cellX >= 0 && cellX <= 23 && cellY >= 0 && cellY <= 23)
      {
        int texture_index = scene->map.floor[cellY][cellX];
        color = scene->textures[scene->map.floor[cellY][cellX]]
                    .pixels[DEFAULT_TEXTURE_HEIGHT * ty + tx];
        pixel_data[(y * (pitch / 4)) + x] = color;

        cellY--;
        if (cellY < 0)
          cellY = 0;

        color = scene->textures[scene->map.ceil[cellY][cellX]]
                    .pixels[DEFAULT_TEXTURE_HEIGHT * ty + tx];
        pixel_data[((h - y) * (pitch / 4)) + x] = color;
      }
      else
      {
        pixel_data[(y * (pitch / 4)) + x] = 0xFF00FFFF;
        pixel_data[((h - y) * (pitch / 4)) + x] = 0xFF00FFFF;
      }
    }
  }
  unlock_texture(texture);
  render_copy(scene->window_ctx->renderer, texture, NULL, NULL);
}

void render_walls(Scene *scene)
{
  Texture *texture = scene->wall_texture;
  set_texture_blend_mode(texture, SDL_BLENDMODE_BLEND);
  void *pixels;
  int pitch;
  lock_texture(texture, NULL, &pixels, &pitch);
  memset(pixels, 0, pitch * scene->window_ctx->window_config->height);
  uint32_t *pixel_data = (uint32_t *)pixels;
  uint32_t color;

  int w, h;
  get_window_size_in_pixels(scene->window_ctx->window, &w, &h);
  for (int x = 0; x < w; x++)
  {
    WallIntersect intersect = scene->player.intersects[x];
    int line_height = (int)h / intersect.perp_wall_distance;
    if (line_height == 0)
      line_height = 1;
    int draw_start = -line_height / 2 + h / 2;
    if (draw_start < 0)
      draw_start = 0;
    int draw_end = line_height / 2 + h / 2;
    if (draw_end >= h)
      draw_end = h - 1;

    if (intersect.map_x < 0 || intersect.map_x > scene->map.width)
      intersect.map_x = 0;
    if (intersect.map_y < 0 || intersect.map_y > scene->map.height)
      intersect.map_y = 0;
    int tex_num = scene->map.walls[intersect.map_y][intersect.map_x] - 1;
    double wall_x;
    if (intersect.side == 0)
      wall_x = intersect.vect.y;
    else
      wall_x = intersect.vect.x;
    wall_x -= floor(wall_x);

    int tex_x = (int)(wall_x * (double)DEFAULT_TEXTURE_WIDTH);
    if (intersect.side == 0 && intersect.ray_dir.x > 0)
      tex_x = DEFAULT_TEXTURE_WIDTH - tex_x - 1;
    if (intersect.side == 1 && intersect.ray_dir.y < 0)
      tex_x = DEFAULT_TEXTURE_WIDTH - tex_x - 1;

    for (int y = draw_start; y <= draw_end; y++)
    {
      int tex_y = (((y * 2 - scene->window_ctx->window_config->height + line_height) * DEFAULT_TEXTURE_HEIGHT) /
                   line_height) /
                  2;

      if (tex_num >= 0)
        color = scene->textures[tex_num].pixels[DEFAULT_TEXTURE_HEIGHT * tex_y + tex_x];
      else
        color = 0xFF00FFFF;

      pixel_data[(y * (pitch / 4)) + x] = color;
    }
  }
  unlock_texture(texture);
  render_copy(scene->window_ctx->renderer, texture, NULL, NULL);
}
