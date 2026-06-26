#include "internal/renderer_int.h"
#include <math.h>
#include <stdlib.h>

static void render_floor_ceiling(RcEngine *e, uint32_t *framebuffer, uint32_t w, uint32_t h)
{
  RcCamera *cam = e->camera;

  for (int y = h / 2; y < h; ++y) {
    float rayDirX0 = cam->dir.x - cam->plane.x;
    float rayDirY0 = cam->dir.y - cam->plane.y;
    float rayDirX1 = cam->dir.x + cam->plane.x;
    float rayDirY1 = cam->dir.y + cam->plane.y;

    int p = y - h / 2;
    if (p == 0)
      p = 1;
    if (p < 0)
      p = 1;

    float posZ = 0.5f * h;
    float rowDistance = posZ / p;
    if (rowDistance < 0.1f)
      rowDistance = 0.1f;

    float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / w;
    float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / w;

    float floorX = cam->pos.x + rowDistance * rayDirX0;
    float floorY = cam->pos.y + rowDistance * rayDirY0;

    for (int x = 0; x < w; x++) {
      int cellX = (int)(floorX);
      int cellY = (int)(floorY);

      int tx = (int)(RC_TEXTURE_WIDTH * (floorX - cellX)) % RC_TEXTURE_WIDTH;
      int ty = (int)(RC_TEXTURE_HEIGHT * (floorY - cellY)) % RC_TEXTURE_HEIGHT;

      floorX += floorStepX;
      floorY += floorStepY;

      if (cellX >= 0 && cellX < 24 && cellY >= 0 && cellY < 24) {
        int floor_tex = 7;
        int ceil_tex = 3;

        uint32_t floor_color = e->textures[floor_tex].pixels[RC_TEXTURE_HEIGHT * ty + tx];
        framebuffer[y * w + x] = floor_color;

        uint32_t ceil_color = e->textures[ceil_tex].pixels[RC_TEXTURE_HEIGHT * ty + tx];
        framebuffer[(h - y - 1) * w + x] = ceil_color;
      } else {
        framebuffer[y * w + x] = 0xFF606078;
        framebuffer[(h - y - 1) * w + x] = 0xFF282840;
      }
    }
  }
}

static void render_sprites(RcEngine *e, uint32_t *framebuffer, double *z_buffer, uint32_t w,
                           uint32_t h)
{
  RcCamera *cam = e->camera;
  RcLevel *world = e->level;

  RcSprite *sprites = NULL;
  uint32_t sprite_count = 0;
  rc_level_get_sprites(world, &sprites, &sprite_count);

  if (sprite_count == 0 || !sprites)
    return;

  double *sprite_dist = malloc(sizeof(double) * sprite_count);
  int *sprite_order = malloc(sizeof(int) * sprite_count);

  for (uint32_t i = 0; i < sprite_count; i++) {
    sprite_order[i] = i;
    sprite_dist[i] = ((cam->pos.x - sprites[i].pos.x) * (cam->pos.x - sprites[i].pos.x) +
                      (cam->pos.y - sprites[i].pos.y) * (cam->pos.y - sprites[i].pos.y));
  }

  for (uint32_t i = 0; i < sprite_count - 1; i++) {
    for (uint32_t j = i + 1; j < sprite_count; j++) {
      if (sprite_dist[j] > sprite_dist[i]) {
        double temp_dist = sprite_dist[i];
        sprite_dist[i] = sprite_dist[j];
        sprite_dist[j] = temp_dist;
        int temp_order = sprite_order[i];
        sprite_order[i] = sprite_order[j];
        sprite_order[j] = temp_order;
      }
    }
  }

  for (uint32_t i = 0; i < sprite_count; i++) {
    RcSprite *sprite = &sprites[sprite_order[i]];

    double sprite_x = sprite->pos.x - cam->pos.x;
    double sprite_y = sprite->pos.y - cam->pos.y;

    double inv_det = 1.0 / (cam->plane.x * cam->dir.y - cam->dir.x * cam->plane.y);
    double transform_x = inv_det * (cam->dir.y * sprite_x - cam->dir.x * sprite_y);
    double transform_y = inv_det * (-cam->plane.y * sprite_x + cam->plane.x * sprite_y);

    if (transform_y <= 0)
      continue;

    int sprite_screen_x = (int)((w / 2) * (1 + transform_x / transform_y));

    int sprite_height = abs((int)(h / transform_y));

    int draw_start_y = -sprite_height / 2 + h / 2;
    if (draw_start_y < 0)
      draw_start_y = 0;
    int draw_end_y = sprite_height / 2 + h / 2;
    if (draw_end_y >= h)
      draw_end_y = h - 1;

    int sprite_width = abs((int)(h / transform_y));

    if (sprite_screen_x < -sprite_width/2)
      continue;

    int draw_start_x = -sprite_width / 2 + sprite_screen_x;
    if (draw_start_x < 0)
      draw_start_x = 0;
    int draw_end_x = sprite_width / 2 + sprite_screen_x;
    if (draw_end_x >= w)
      draw_end_x = w - 1;

    int tex_num = 8 + (sprite->texture_id % 3);

    for (int stripe = draw_start_x; stripe < draw_end_x; stripe++) {
      int tex_x =
          (int)((stripe - (-sprite_width / 2 + sprite_screen_x)) * RC_TEXTURE_WIDTH / sprite_width);

      if (transform_y < z_buffer[stripe]) {
        for (int y = draw_start_y; y < draw_end_y; y++) {
          int tex_y = (((y * 2 - h + sprite_height) * RC_TEXTURE_HEIGHT) / sprite_height) / 2;

          uint32_t color = e->textures[tex_num].pixels[RC_TEXTURE_HEIGHT * tex_y + tex_x];

          if ((color & 0xFF) == 0xFF) {
            framebuffer[y * w + stripe] = color;
          }
        }
      }
    }
  }

  free(sprite_dist);
  free(sprite_order);
}

static void render_walls(RcEngine *e, uint32_t *framebuffer, double *z_buffer, uint32_t w,
                         uint32_t h)
{
  RcCamera *cam = e->camera;
  RcLevel *world = e->level;

  for (int x = 0; x < w; x++) {
    double camera_x = 2 * x / (double)w - 1.0;
    double ray_dir_x = cam->dir.x + cam->plane.x * camera_x;
    double ray_dir_y = cam->dir.y + cam->plane.y * camera_x;

    int map_x = (int)cam->pos.x;
    int map_y = (int)cam->pos.y;

    double side_dist_x, side_dist_y;
    double delta_dist_x = (ray_dir_x == 0) ? 1e30 : fabs(1 / ray_dir_x);
    double delta_dist_y = (ray_dir_y == 0) ? 1e30 : fabs(1 / ray_dir_y);
    double perp_wall_dist;

    int step_x, step_y;
    int hit = 0;
    int side;

    if (ray_dir_x < 0) {
      step_x = -1;
      side_dist_x = (cam->pos.x - map_x) * delta_dist_x;
    } else {
      step_x = 1;
      side_dist_x = (map_x + 1.0 - cam->pos.x) * delta_dist_x;
    }

    if (ray_dir_y < 0) {
      step_y = -1;
      side_dist_y = (cam->pos.y - map_y) * delta_dist_y;
    } else {
      step_y = 1;
      side_dist_y = (map_y + 1.0 - cam->pos.y) * delta_dist_y;
    }

    while (hit == 0) {
      if (side_dist_x < side_dist_y) {
        side_dist_x += delta_dist_x;
        map_x += step_x;
        side = 0;
      } else {
        side_dist_y += delta_dist_y;
        map_y += step_y;
        side = 1;
      }

      if (rc_level_get_wall(world, map_x, map_y) > 0)
        hit = 1;
    }

    if (side == 0)
      perp_wall_dist = (side_dist_x - delta_dist_x);
    else
      perp_wall_dist = (side_dist_y - delta_dist_y);

    z_buffer[x] = perp_wall_dist;

    int line_height = (int)(h / perp_wall_dist);
    if (line_height == 0)
      line_height = 1;

    int draw_start = -line_height / 2 + h / 2;
    if (draw_start < 0)
      draw_start = 0;

    int draw_end = line_height / 2 + h / 2;
    if (draw_end >= h)
      draw_end = h - 1;

    uint32_t wall_type = rc_level_get_wall(world, map_x, map_y);
    int tex_num = (int)wall_type - 1;

    double wall_x;
    if (side == 0)
      wall_x = cam->pos.y + perp_wall_dist * ray_dir_y;
    else
      wall_x = cam->pos.x + perp_wall_dist * ray_dir_x;
    wall_x -= floor(wall_x);

    int tex_x = (int)(wall_x * RC_TEXTURE_WIDTH);
    if (side == 0 && ray_dir_x > 0)
      tex_x = RC_TEXTURE_WIDTH - tex_x - 1;
    if (side == 1 && ray_dir_y < 0)
      tex_x = RC_TEXTURE_WIDTH - tex_x - 1;

    for (int y = draw_start; y <= draw_end; y++) {
      int tex_y = (((y * 2 - h + line_height) * RC_TEXTURE_HEIGHT) / line_height) / 2;

      uint32_t color;
      if (tex_num >= 0 && e->textures && tex_num < 11) {
        color = e->textures[tex_num].pixels[RC_TEXTURE_HEIGHT * tex_y + tex_x];
      } else {
        color = 0xFF00FFFF;
      }

      if (side == 1) {
        uint8_t r, g, b, a;
        r = (color >> 24) & 0xFF;
        g = (color >> 16) & 0xFF;
        b = (color >> 8) & 0xFF;
        a = color & 0xFF;
        r = (r * 0.7);
        g = (g * 0.7);
        b = (b * 0.7);
        color = (r << 24) | (g << 16) | (b << 8) | a;
      }

      framebuffer[y * w + x] = color;
    }
  }
}

void rc_render_software_frame(RcEngine *e, uint32_t *framebuffer, double *z_buffer)
{
  uint32_t w = e->config.rend_config.width;
  uint32_t h = e->config.rend_config.height;
  render_floor_ceiling(e, framebuffer, w, h);
  render_walls(e, framebuffer, z_buffer, w, h);
  render_sprites(e, framebuffer, z_buffer, w, h);
}
