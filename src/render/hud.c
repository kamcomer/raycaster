#include "hud.h"
#include "renderer.h"
#include "texture.h"
#include <math.h>

#define MINIMAP_SIZE 150
#define MINIMAP_SCALE 6
#define MINIMAP_MARGIN 10

void render_minimap(Scene *scene)
{
  Renderer *rend = scene->window_ctx->renderer;
  WindowConfig *win_cfg = scene->window_ctx->window_config;

  int map_x = win_cfg->width - MINIMAP_SIZE - MINIMAP_MARGIN;
  int map_y = MINIMAP_MARGIN;

  set_render_draw_color(rend, 0, 0, 0, 180);
  Rect bg = {map_x, map_y, MINIMAP_SIZE, MINIMAP_SIZE};
  render_fill_rect(rend, &bg);

  set_render_draw_color(rend, 255, 255, 255, 255);
  for (int y = 0; y < scene->map.height; y++)
  {
    for (int x = 0; x < scene->map.width; x++)
    {
      if (scene->map.walls[y][x])
      {
        Rect wall = {map_x + x * MINIMAP_SCALE, map_y + y * MINIMAP_SCALE,
                     MINIMAP_SCALE, MINIMAP_SCALE};
        render_fill_rect(rend, &wall);
      }
    }
  }

  for (int i = 0; i < scene->static_sprites.num_sprites; i++)
  {
    int sx = (int)(scene->static_sprites.sprites[i].pos.x);
    int sy = (int)(scene->static_sprites.sprites[i].pos.y);
    if (sx >= 0 && sx < scene->map.width && sy >= 0 && sy < scene->map.height)
    {
      set_render_draw_color(rend, 255, 255, 0, 255);
      Rect sprite = {map_x + sx * MINIMAP_SCALE, map_y + sy * MINIMAP_SCALE,
                     MINIMAP_SCALE, MINIMAP_SCALE};
      render_fill_rect(rend, &sprite);
    }
  }

  int player_map_x = map_x + (int)(scene->player.actor->pos.x / scene->map.unit_size) * MINIMAP_SCALE;
  int player_map_y = map_y + (int)(scene->player.actor->pos.y / scene->map.unit_size) * MINIMAP_SCALE;

  set_render_draw_color(rend, 0, 255, 0, 255);
  Rect player = {player_map_x - 2, player_map_y - 2, 4, 4};
  render_fill_rect(rend, &player);

  double dir_angle = scene->player.actor->dir.angle;
  int dir_end_x = player_map_x + (int)(cos(dir_angle) * 10);
  int dir_end_y = player_map_y + (int)(sin(dir_angle) * 10);
  set_render_draw_color(rend, 255, 0, 0, 255);
  render_draw_line(rend, player_map_x, player_map_y, dir_end_x, dir_end_y);
}

void render_crosshair(Scene *scene)
{
  Renderer *rend = scene->window_ctx->renderer;
  WindowConfig *win_cfg = scene->window_ctx->window_config;

  int center_x = win_cfg->width / 2;
  int center_y = win_cfg->height / 2;
  int size = 10;

  set_render_draw_color(rend, 255, 255, 255, 200);

  render_draw_line(rend, center_x - size, center_y, center_x + size, center_y);
  render_draw_line(rend, center_x, center_y - size, center_x, center_y + size);
}

void render_hud(Scene *scene)
{
  render_minimap(scene);
  render_crosshair(scene);
}
