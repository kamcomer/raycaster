#include "scene.h"
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

  // Initialize map
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

  // Set the map and player for the scene
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

void render_2d_scene(Scene *scene)
{
  render_2d_map(*scene);
  render_2d_player(scene);
}

void render_2d_map(Scene scene)
{
  int x_offset;
  int y_offset;

  // Set the background color (white)
  set_render_draw_color(scene.window_ctx->renderer, 255, 255, 255, 255);
  clear_renderer(scene.window_ctx->renderer);

  // Loop through the map and draw rectangles
  for (int y = 0; y < scene.map.height; y++)
  {
    y_offset = (y == scene.map.height - 1) ? 0 : 1;

    for (int x = 0; x < scene.map.width; x++)
    {
      x_offset = (x == scene.map.width - 1) ? 0 : 1;

      // Set the color depending on the grid value
      if (scene.map.walls[y][x])
      {
        set_render_draw_color(scene.window_ctx->renderer, 255, 0, 0, 255); // Red for filled cells
      }
      else
      {
        set_render_draw_color(scene.window_ctx->renderer, 0, 0, 0, 255); // Black for empty cells
      }

      // Define the rectangle for each cell in the map
      Rect rectangle = {x * scene.map.unit_size, y * scene.map.unit_size,
                        scene.map.unit_size - x_offset,
                        scene.map.unit_size - y_offset};

      // Draw the rectangle onto the texture
      render_fill_rect(scene.window_ctx->renderer, &rectangle);
    }
  }
}

void render_2d_player(Scene *scene)
{
  render_actor_body(scene);

#ifdef DEBUG
  render_player_view_rays(scene);
  render_actor_view_dir(scene);
  render_actor_vel_dir(scene);
  render_player_plane(scene);
#endif
}

void render_player_plane(Scene *scene)
{
  Player player = scene->player;
  set_render_draw_color(scene->window_ctx->renderer, 0, 0, 0, 255);
  set_vector_magnitude(&player.actor->dir, 10);
  set_vector_magnitude(&player.plane, 5);
  render_draw_line(scene->window_ctx->renderer,
                   player.actor->pos.x + player.actor->dir.x - player.plane.x,
                   player.actor->pos.y + player.actor->dir.y - player.plane.y,
                   player.actor->pos.x + player.actor->dir.x + player.plane.x,
                   player.actor->pos.y + player.actor->dir.y + player.plane.y);
}

void render_actor_body(Scene *scene)
{
  Actor actor = *scene->player.actor;
  set_render_draw_color(scene->window_ctx->renderer, 0, 255, 0, 255);
  Rect rect = {actor.pos.x - (actor.size >> 1),
               actor.pos.y - (actor.size >> 1), actor.size, actor.size};
  render_fill_rect(scene->window_ctx->renderer, &rect);
}

void render_actor_view_dir(Scene *scene)
{
  Actor actor = *scene->player.actor;
  set_render_draw_color(scene->window_ctx->renderer, 0, 0, 0, 255);
  set_vector_magnitude(&actor.dir, 10);
  translate_vector(&actor.dir, actor.pos);
  render_draw_line(scene->window_ctx->renderer,
                   actor.pos.x, actor.pos.y, actor.dir.x, actor.dir.y);
}

void render_actor_vel_dir(Scene *scene)
{
  Actor actor = *scene->player.actor;
  set_render_draw_color(scene->window_ctx->renderer, 255, 255, 255, 255);
  set_vector_magnitude(&actor.velocity, 10);
  Vector vel_endpoint = set_vector(actor.pos.x + actor.velocity.x,
                                   actor.pos.y + actor.velocity.y);
  render_draw_line(scene->window_ctx->renderer,
                   actor.pos.x, actor.pos.y, vel_endpoint.x, vel_endpoint.y);
}

void render_actor_view_rays(Scene *scene)
{
  Actor actor = *scene->player.actor;
  set_render_draw_color(scene->window_ctx->renderer, 255, 0, 255, 75);
  for (int i = 0; i < DEFAULT_NUM_RAYS; i++)
  {
    Vector ray = actor.view_cone[i];
    render_draw_line(scene->window_ctx->renderer,
                     actor.pos.x, actor.pos.y, ray.x, ray.y);
  }
}

void render_player_view_rays(Scene *scene)
{
  Player player = scene->player;
  set_render_draw_color(scene->window_ctx->renderer, 255, 0, 255, 75);
  for (int i = 0; i < scene->window_ctx->window_config->width; i++)
  {
    Vector ray = player.intersects[i].vect;
    render_draw_line(scene->window_ctx->renderer,
                     player.actor->pos.x, player.actor->pos.y,
                     ray.x * DEFAULT_MAP_UNIT_SIZE, ray.y * DEFAULT_MAP_UNIT_SIZE);
  }
}

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

  Texture *texture = create_texture(
      scene->window_ctx->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
      scene->window_ctx->window_config->width,
      scene->window_ctx->window_config->height);
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
  destroy_texture(texture);
}

void render_floor_and_ceil(Scene *scene)
{
  Texture *texture = create_texture(
      scene->window_ctx->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
      scene->window_ctx->window_config->width, scene->window_ctx->window_config->height);
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
  destroy_texture(texture);
}

void render_walls(Scene *scene)
{
  Texture *texture = create_texture(
      scene->window_ctx->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
      scene->window_ctx->window_config->width, scene->window_ctx->window_config->height);
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

    // Calculate the x coordinate on the texture (from 0 to DEFAULT_TEXTURE_WIDTH - 1)
    int tex_x = (int)(wall_x * (double)DEFAULT_TEXTURE_WIDTH);
    if (intersect.side == 0 && intersect.ray_dir.x > 0)
      tex_x = DEFAULT_TEXTURE_WIDTH - tex_x - 1;
    if (intersect.side == 1 && intersect.ray_dir.y < 0)
      tex_x = DEFAULT_TEXTURE_WIDTH - tex_x - 1;

    // Draw the vertical walls slice
    for (int y = draw_start; y <= draw_end; y++)
    {
      // Calculate the corresponding y position on the texture
      int tex_y = (((y * 2 - scene->window_ctx->window_config->height + line_height) * DEFAULT_TEXTURE_HEIGHT) /
                   line_height) /
                  2;

      // Get the color from the texture
      if (tex_num >= 0)
        color = scene->textures[tex_num].pixels[DEFAULT_TEXTURE_HEIGHT * tex_y + tex_x];
      else
        color = 0xFF00FFFF;

      // Modify color for shadows if hitting a horizontal walls
      // if (intersect.side == 1)
      //   color = ((color >> 9) & 0x7F7F7F) << 8 | 0xFF; // Darken the color

      // Set the pixel color and draw the pixel
      pixel_data[(y * (pitch / 4)) + x] = color;
    }
  }
  unlock_texture(texture);
  render_copy(scene->window_ctx->renderer, texture, NULL, NULL);
  destroy_texture(texture);
}

void free_scene(Scene *scene)
{
  free_player(&scene->player);
  free_map(scene->map);
  free(scene);
}
