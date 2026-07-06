#include "alpha.h"
#include "internal/i_alpha.h"
#include <math.h>
#include <stdlib.h>

static void render_floor_ceiling(const AScene *scene, const ADimensions *window_dims,
                                 uint32 *framebuffer)
{
  ACamera cam = scene->camera;
  uint height = window_dims->height;
  uint width = window_dims->width;

  for (uint y = height / 2; y < height; ++y) {
    float rayDirX0 = cam.dir.x - cam.plane.x;
    float rayDirY0 = cam.dir.y - cam.plane.y;
    float rayDirX1 = cam.dir.x + cam.plane.x;
    float rayDirY1 = cam.dir.y + cam.plane.y;

    int p = y - height / 2;
    if (p == 0)
      p = 1;
    if (p < 0)
      p = 1;

    float posZ = 0.5f * height;
    float rowDistance = posZ / p;
    if (rowDistance < 0.1f)
      rowDistance = 0.1f;

    float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / width;
    float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / width;

    float floorX = cam.pos.x + rowDistance * rayDirX0;
    float floorY = cam.pos.y + rowDistance * rayDirY0;

    for (uint32_t x = 0; x < width; x++) {
      int cellX = (int)(floorX);
      int cellY = (int)(floorY);

      int tx = (int)(A_TEXTURE_WIDTH * (floorX - cellX)) % A_TEXTURE_WIDTH;
      int ty = (int)(A_TEXTURE_HEIGHT * (floorY - cellY)) % A_TEXTURE_HEIGHT;

      floorX += floorStepX;
      floorY += floorStepY;

      if (cellX >= 0 && cellX < 24 && cellY >= 0 && cellY < 24) {
        int floor_tex = 7;
        int ceil_tex = 7;

        uint32_t floor_color = scene->textures->items[floor_tex].pixels[A_TEXTURE_HEIGHT * ty + tx];
        framebuffer[y * width + x] = floor_color;

        uint32_t ceil_color = scene->textures->items[ceil_tex].pixels[A_TEXTURE_HEIGHT * ty + tx];
        framebuffer[(height - y - 1) * width + x] = ceil_color;
      } else {
        framebuffer[y * width + x] = 0xFF606078;
        framebuffer[(height - y - 1) * width + x] = 0xFF282840;
      }
    }
  }
}

static void render_sprites(const AScene *scene, const ADimensions *window_dims, uint32 *framebuffer,
                           const double *z_buffer)
{
  ACamera cam = scene->camera;
  ALevel *level = scene->level;
  uint height = window_dims->height;
  uint width = window_dims->width;

  ASprite *sprites = NULL;
  uint sprite_count = 0;
  a_level_get_sprites(level, &sprites, &sprite_count);

  if (sprite_count == 0 || !sprites) {
    return;
  }

  double *sprite_dist = malloc(sizeof(double) * sprite_count);
  int *sprite_order = malloc(sizeof(int) * sprite_count);

  for (uint32_t i = 0; i < sprite_count; i++) {
    sprite_order[i] = i;
    sprite_dist[i] = ((cam.pos.x - sprites[i].pos.x) * (cam.pos.x - sprites[i].pos.x) +
                      (cam.pos.y - sprites[i].pos.y) * (cam.pos.y - sprites[i].pos.y));
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

  for (uint32 i = 0; i < sprite_count; i++) {
    ASprite *sprite = &sprites[sprite_order[i]];

    double sprite_x = sprite->pos.x - cam.pos.x;
    double sprite_y = sprite->pos.y - cam.pos.y;

    double inv_det = 1.0 / (cam.plane.x * cam.dir.y - cam.dir.x * cam.plane.y);
    double transform_x = inv_det * (cam.dir.y * sprite_x - cam.dir.x * sprite_y);
    double transform_y = inv_det * (-cam.plane.y * sprite_x + cam.plane.x * sprite_y);

    if (transform_y <= 0)
      continue;

    int sprite_screen_x = (int)((width / 2) * (1 + transform_x / transform_y));

    int sprite_height = abs((int)(height / transform_y));

    int draw_start_y = -sprite_height / 2 + height / 2;
    if (draw_start_y < 0)
      draw_start_y = 0;
    int draw_end_y = sprite_height / 2 + height / 2;
    if (draw_end_y >= (int)height)
      draw_end_y = height - 1;

    int sprite_width = abs((int)(height / transform_y));

    if (sprite_screen_x < -sprite_width / 2)
      continue;

    int draw_start_x = -sprite_width / 2 + sprite_screen_x;
    if (draw_start_x < 0)
      draw_start_x = 0;
    int draw_end_x = sprite_width / 2 + sprite_screen_x;
    if (draw_end_x >= (int)width)
      draw_end_x = width - 1;

    int tex_num = 8 + (sprite->texture_id % 3);

    for (int stripe = draw_start_x; stripe < draw_end_x; stripe++) {
      int tex_x =
          (int)((stripe - (-sprite_width / 2 + sprite_screen_x)) * A_TEXTURE_WIDTH / sprite_width);

      if (transform_y < z_buffer[stripe]) {
        for (int y = draw_start_y; y < draw_end_y; y++) {
          int tex_y = (((y * 2 - height + sprite_height) * A_TEXTURE_HEIGHT) / sprite_height) / 2;

          uint32_t color = scene->textures->items[tex_num].pixels[A_TEXTURE_HEIGHT * tex_y + tex_x];

          if ((color & 0xFF) == 0xFF) {
            framebuffer[y * width + stripe] = color;
          }
        }
      }
    }
  }

  free(sprite_dist);
  free(sprite_order);
}

static void render_walls(const AScene *scene, const ADimensions *window_dims, uint32 *framebuffer,
                         double *z_buffer)
{
  ACamera cam = scene->camera;
  ALevel *level = scene->level;
  uint height = window_dims->height;
  uint width = window_dims->width;

  for (uint32_t x = 0; x < width; x++) {
    double camera_x = 2 * x / (double)width - 1.0;
    double ray_dir_x = cam.dir.x + cam.plane.x * camera_x;
    double ray_dir_y = cam.dir.y + cam.plane.y * camera_x;

    int map_x = (int)cam.pos.x;
    int map_y = (int)cam.pos.y;

    double side_dist_x, side_dist_y;
    double delta_dist_x = (ray_dir_x == 0) ? 1e30 : fabs(1 / ray_dir_x);
    double delta_dist_y = (ray_dir_y == 0) ? 1e30 : fabs(1 / ray_dir_y);
    double perp_wall_dist;

    int step_x, step_y;
    int hit = 0;
    int side;

    if (ray_dir_x < 0) {
      step_x = -1;
      side_dist_x = (cam.pos.x - map_x) * delta_dist_x;
    } else {
      step_x = 1;
      side_dist_x = (map_x + 1.0 - cam.pos.x) * delta_dist_x;
    }

    if (ray_dir_y < 0) {
      step_y = -1;
      side_dist_y = (cam.pos.y - map_y) * delta_dist_y;
    } else {
      step_y = 1;
      side_dist_y = (map_y + 1.0 - cam.pos.y) * delta_dist_y;
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

      if (a_level_get_wall(level, map_x, map_y) > 0)
        hit = 1;
    }

    if (side == 0)
      perp_wall_dist = (side_dist_x - delta_dist_x);
    else
      perp_wall_dist = (side_dist_y - delta_dist_y);

    z_buffer[x] = perp_wall_dist;

    int line_height = (int)(height / perp_wall_dist);
    if (line_height == 0)
      line_height = 1;

    int draw_start = -line_height / 2 + height / 2;
    if (draw_start < 0)
      draw_start = 0;

    int draw_end = line_height / 2 + height / 2;
    if (draw_end >= (int)height)
      draw_end = height - 1;

    uint32_t wall_type = a_level_get_wall(level, map_x, map_y);
    int tex_num = (int)wall_type - 1;

    double wall_x;
    if (side == 0)
      wall_x = cam.pos.y + perp_wall_dist * ray_dir_y;
    else
      wall_x = cam.pos.x + perp_wall_dist * ray_dir_x;
    wall_x -= floor(wall_x);

    int tex_x = (int)(wall_x * A_TEXTURE_WIDTH);
    if (side == 0 && ray_dir_x > 0)
      tex_x = A_TEXTURE_WIDTH - tex_x - 1;
    if (side == 1 && ray_dir_y < 0)
      tex_x = A_TEXTURE_WIDTH - tex_x - 1;

    for (int y = draw_start; y <= draw_end; y++) {
      int tex_y = (((y * 2 - height + line_height) * A_TEXTURE_HEIGHT) / line_height) / 2;

      uint32_t color;
      if (tex_num >= 0 && scene->textures && tex_num < 11) {
        color = scene->textures->items[tex_num].pixels[A_TEXTURE_HEIGHT * tex_y + tex_x];
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

      framebuffer[y * width + x] = color;
    }
  }
}

static void a_renderer_technique_raycaster_process(ARendererTechnique *technique)
{
  ARendererTechniqueRaycasterData *data = technique->impl;
  render_floor_ceiling(data->scene, data->window_dimensions, data->framebuffer);
  render_walls(data->scene, data->window_dimensions, data->framebuffer, data->zbuffer);
  render_sprites(data->scene, data->window_dimensions, data->framebuffer, data->zbuffer);
}

static ARendererTechniqueVtbl renderer_technique_raycaster = {
    .process = a_renderer_technique_raycaster_process};

AResult a_renderer_technique_raycaster_init(const ARendererTechniqueRaycasterData data_in,
                                            ARendererTechnique *out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = (ARendererTechnique){0};
  ARendererTechniqueRaycasterData *data = malloc(sizeof(ARendererTechniqueRaycasterData));
  if (!data) {
    return A_RES_ALLOC_ERR;
  }

  *data = data_in;
  out->impl = data;
  out->vtbl = &renderer_technique_raycaster;

  return A_RES_OK;
}

AResult a_renderer_technique_raycaster_create(const ARendererTechniqueRaycasterData data_in,
                                              ARendererTechnique **out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = NULL;

  ARendererTechnique *technique = malloc(sizeof(ARendererTechnique));
  if (!technique) {
    return A_RES_INVLD_ARG;
  }

  AResult res = a_renderer_technique_raycaster_init(data_in, technique);
  if (res != A_RES_OK) {
    free(technique);
    return res;
  }

  *out = technique;
  return A_RES_OK;
}

void a_renderer_technique_raycaster_deinit(ARendererTechnique *technique)
{
  if (!technique) {
    return;
  }
  free(technique->impl);
}

void a_renderer_technique_raycaster_destroy(ARendererTechnique *technique)
{
  if (!technique) {
    return;
  }
  a_renderer_technique_raycaster_deinit(technique);
  free(technique);
}
