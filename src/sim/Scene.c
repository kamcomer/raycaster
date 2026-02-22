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

static Scene *current_scene = NULL;

void set_current_scene(Scene *scene)
{
  current_scene = scene;
}

Scene *get_current_scene()
{
  return current_scene;
}

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

void render_scene(void (*render)())
{
  set_render_draw_color(current_scene->window_ctx->renderer, 0, 0, 0, 255);
  clear_renderer(current_scene->window_ctx->renderer);
  render(*current_scene);
}

void render_2d_scene(void)
{
  render_2d_map(*current_scene);
  render_2d_player();
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

void render_2d_player()
{
  render_actor_body(*current_scene->player.actor);

#ifdef DEBUG
  render_player_view_rays(player);
  render_actor_view_dir(player.actor);
  render_actor_vel_dir(player.actor);
  render_player_plane(player);
#endif
}

void render_player_plane(Player player)
{
  set_render_draw_color(current_scene->window_ctx->renderer, 0, 0, 0, 255);
  set_vector_magnitude(&player.actor->dir, 10);
  set_vector_magnitude(&player.plane, 5);
  render_draw_line(current_scene->window_ctx->renderer,
                   player.actor->pos.x + player.actor->dir.x - player.plane.x,
                   player.actor->pos.y + player.actor->dir.y - player.plane.y,
                   player.actor->pos.x + player.actor->dir.x + player.plane.x,
                   player.actor->pos.y + player.actor->dir.y + player.plane.y);
}

void render_actor_body(Actor actor)
{
  set_render_draw_color(current_scene->window_ctx->renderer, 0, 255, 0, 255);
  Rect rect = {actor.pos.x - (actor.size >> 1),
               actor.pos.y - (actor.size >> 1), actor.size, actor.size};
  render_fill_rect(current_scene->window_ctx->renderer, &rect);
}

void render_actor_view_dir(Actor actor)
{
  set_render_draw_color(current_scene->window_ctx->renderer, 0, 0, 0, 255);
  set_vector_magnitude(&actor.dir, 10);
  translate_vector(&actor.dir, actor.pos);
  render_draw_line(current_scene->window_ctx->renderer,
                   actor.pos.x, actor.pos.y, actor.dir.x, actor.dir.y);
}

void render_actor_vel_dir(Actor actor)
{
  set_render_draw_color(current_scene->window_ctx->renderer, 255, 255, 255, 255);
  set_vector_magnitude(&actor.velocity, 10);
  Vector vel_endpoint = set_vector(actor.pos.x + actor.velocity.x,
                                   actor.pos.y + actor.velocity.y);
  render_draw_line(current_scene->window_ctx->renderer,
                   actor.pos.x, actor.pos.y, vel_endpoint.x, vel_endpoint.y);
}

void render_actor_view_rays(Actor actor)
{
  set_render_draw_color(current_scene->window_ctx->renderer, 255, 0, 255, 75);
  for (int i = 0; i < DEFAULT_NUM_RAYS; i++)
  {
    Vector ray = actor.view_cone[i];
    render_draw_line(current_scene->window_ctx->renderer,
                     actor.pos.x, actor.pos.y, ray.x, ray.y);
  }
}

void render_player_view_rays(Player player)
{
  set_render_draw_color(current_scene->window_ctx->renderer, 255, 0, 255, 75);
  for (int i = 0; i < current_scene->window_ctx->window_config->width; i++)
  {
    Vector ray = player.intersects[i].vect;
    render_draw_line(current_scene->window_ctx->renderer,
                     player.actor->pos.x, player.actor->pos.y,
                     ray.x * DEFAULT_MAP_UNIT_SIZE, ray.y * DEFAULT_MAP_UNIT_SIZE);
  }
}

void render_fp_scene(void)
{
  render_floor_and_ceil();
  render_walls();
  renderer_sprites();
  render_hud(current_scene);
}

void renderer_sprites()
{
  Vector player_pos = current_scene->player.actor->pos;
  player_pos.x /= DEFAULT_MAP_UNIT_SIZE;
  player_pos.y /= DEFAULT_MAP_UNIT_SIZE;

  Texture *texture = create_texture(
      current_scene->window_ctx->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
      current_scene->window_ctx->window_config->width,
      current_scene->window_ctx->window_config->height);
  set_texture_blend_mode(texture, SDL_BLENDMODE_BLEND);

  void *pixels;
  int pitch;
  lock_texture(texture, NULL, &pixels, &pitch);
  memset(pixels, 0xFFFFFF00, pitch * current_scene->window_ctx->window_config->height);
  uint32_t *pixel_data = (uint32_t *)pixels;
  uint32_t color;

  SceneStaticSprites static_sprites = current_scene->static_sprites;

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

    double inv_det = 1.0 / (current_scene->player.plane.x * current_scene->player.actor->dir.y -
                            current_scene->player.actor->dir.x * current_scene->player.plane.y);
    Vector transform =
        set_vector(inv_det * (current_scene->player.actor->dir.y * rel_sprite_pos.x -
                              current_scene->player.actor->dir.x * rel_sprite_pos.y),
                   inv_det * (-current_scene->player.plane.y * rel_sprite_pos.x +
                              current_scene->player.plane.x * rel_sprite_pos.y));
    int sprite_screen_x =
        (int)((current_scene->window_ctx->window_config->width / 2.0) * (1 + transform.x / transform.y));
    int sprite_height = abs((int)(current_scene->window_ctx->window_config->height / transform.y));

    int draw_start_y = -sprite_height / 2 + current_scene->window_ctx->window_config->height / 2;
    if (draw_start_y < 0)
      draw_start_y = 0;

    int draw_end_y = sprite_height / 2 + current_scene->window_ctx->window_config->height / 2;
    if (draw_end_y >= current_scene->window_ctx->window_config->height)
      draw_end_y = current_scene->window_ctx->window_config->height - 1;

    int sprite_width = abs((int)(current_scene->window_ctx->window_config->height / transform.y));
    int draw_start_x = -sprite_width / 2 + sprite_screen_x;
    if (draw_start_x < 0)
      draw_start_x = 0;

    int draw_end_x = sprite_width / 2 + sprite_screen_x;
    if (draw_end_x >= current_scene->window_ctx->window_config->width)
      draw_end_x = current_scene->window_ctx->window_config->width - 1;

    for (int stripe = draw_start_x; stripe < draw_end_x; stripe++)
    {
      int tex_x = (int)(256 * (stripe - (-sprite_width / 2 + sprite_screen_x)) *
                        DEFAULT_TEXTURE_WIDTH / sprite_width) /
                  256;
      if (transform.y > 0 && stripe > 0 && stripe < current_scene->window_ctx->window_config->width &&
          transform.y < current_scene->player.intersects[stripe].perp_wall_distance)
      {
        for (int y = draw_start_y; y < draw_end_y; y++)
        {
          int d = (y) * 256 - current_scene->window_ctx->window_config->height * 128 + sprite_height * 128;
          int tex_y = ((d * DEFAULT_TEXTURE_HEIGHT) / sprite_height) / 256;
          color = current_scene
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
  render_copy(current_scene->window_ctx->renderer, texture, NULL, NULL);
  destroy_texture(texture);
}

void render_floor_and_ceil()
{
  Texture *texture = create_texture(
      current_scene->window_ctx->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
      current_scene->window_ctx->window_config->width, current_scene->window_ctx->window_config->height);
  void *pixels;
  int pitch;
  lock_texture(texture, NULL, &pixels, &pitch);
  memset(pixels, 0xFFFFFF00, pitch * current_scene->window_ctx->window_config->height);
  uint32_t *pixel_data = (uint32_t *)pixels;
  uint32_t color;

  int w, h;
  get_window_size_in_pixels(current_scene->window_ctx->window, &w, &h);
  for (int y = h / 2; y < h; ++y)
  {
    // rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
    float rayDirX0 = current_scene->player.actor->dir.x - current_scene->player.plane.x;
    float rayDirY0 = current_scene->player.actor->dir.y - current_scene->player.plane.y;
    float rayDirX1 = current_scene->player.actor->dir.x + current_scene->player.plane.x;
    float rayDirY1 = current_scene->player.actor->dir.y + current_scene->player.plane.y;

    // Current y position compared to the center of the screen (the horizon)
    int p = y - h / 2;
    if (p == 0)
      p = 1; // Prevent division by zero
    if (p < 0)
      p = 1; // Avoid negative p values

    // Vertical position of the camera.
    // NOTE: with 0.5, it's exactly in the center between floor and ceiling,
    // matching also how the walls are being raycasted. For different values
    // than 0.5, a separate loop must be done for ceiling and floor since
    // they're no longer symmetrical.
    float posZ = 0.5 * h;

    // Horizontal distance from the camera to the floor for the current row.
    // 0.5 is the z position exactly in the middle between floor and ceiling.
    // NOTE: this is affine textures mapping, which is not perspective correct
    // except for perfectly horizontal and vertical surfaces like the floor.
    // NOTE: this formula is explained as follows: The camera ray goes through
    // the following two points: the camera itself, which is at a certain
    // height (posZ), and a point in front of the camera (through an imagined
    // vertical plane containing the screen pixels) with horizontal distance
    // 1 from the camera, and vertical position p lower than posZ (posZ - p).
    // When going through that point, the line has vertically traveled by p
    // units and horizontally by 1 unit. To hit the floor, it instead needs to
    // travel by posZ units. It will travel the same ratio horizontally. The
    // ratio was 1 / p for going through the camera plane, so to go posZ times
    // farther to reach the floor, we get that the total horizontal distance is
    // posZ / p.
    float rowDistance = posZ / p;

    // calculate the real world step vector we have to add for each x (parallel
    // to camera plane) adding step by step avoids multiplications with a weight
    // in the inner loop
    float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / w;
    float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / w;

    // real world coordinates of the leftmost column. This will be updated as we
    // step to the right.
    float floorX =
        current_scene->player.actor->pos.x / DEFAULT_MAP_UNIT_SIZE + rowDistance * rayDirX0;
    float floorY =
        current_scene->player.actor->pos.y / DEFAULT_MAP_UNIT_SIZE + rowDistance * rayDirY0;

    for (int x = 0; x < w; x++)
    {
      // the cell coord is simply got from the integer parts of floorX and
      // floorY
      int cellX = (int)(floorX);
      int cellY = (int)(floorY);

      // if (x % 2 && y == game_window.height / 2 + 1)
      //   printf("y: %d, x: %d, floorX: %f, floorY: %f, floorStepX: %f,
      //   floorStepY: %f, cellX: %d, cellY: %d\n", y, x, floorX, floorY,
      //   floorStepX, floorStepY, cellX, cellY);

      // get the textures coordinate from the fractional part
      int tx = (int)(DEFAULT_TEXTURE_WIDTH * (floorX - cellX)) % DEFAULT_TEXTURE_WIDTH;
      int ty = (int)(DEFAULT_TEXTURE_HEIGHT * (floorY - cellY)) % DEFAULT_TEXTURE_HEIGHT;

      // if (x == 0)
      //   printf("y: %d, x: %d, tx: %d, ty: %d\n", y, x, tx, ty);

      floorX += floorStepX;
      floorY += floorStepY;

      // floor
      if (cellX >= 0 && cellX <= 23 && cellY >= 0 && cellY <= 23)
      {
        // Floor
        int texture_index = current_scene->map.floor[cellY][cellX];
        color = current_scene->textures[current_scene->map.floor[cellY][cellX]]
                    .pixels[DEFAULT_TEXTURE_HEIGHT * ty + tx];
        pixel_data[(y * (pitch / 4)) + x] = color;

        // Ceil
        cellY--;
        if (cellY < 0)
          cellY = 0;

        color = current_scene->textures[current_scene->map.ceil[cellY][cellX]]
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
  render_copy(current_scene->window_ctx->renderer, texture, NULL, NULL);
  destroy_texture(texture);
}

void render_walls()
{
  Texture *texture = create_texture(
      current_scene->window_ctx->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
      current_scene->window_ctx->window_config->width, current_scene->window_ctx->window_config->height);
  set_texture_blend_mode(texture, SDL_BLENDMODE_BLEND);
  void *pixels;
  int pitch;
  lock_texture(texture, NULL, &pixels, &pitch);
  memset(pixels, 0, pitch * current_scene->window_ctx->window_config->height);
  uint32_t *pixel_data = (uint32_t *)pixels;
  uint32_t color;

  int w, h;
  get_window_size_in_pixels(current_scene->window_ctx->window, &w, &h);
  for (int x = 0; x < w; x++)
  {
    WallIntersect intersect = current_scene->player.intersects[x];
    int line_height = (int)h / intersect.perp_wall_distance;
    if (line_height == 0)
      line_height = 1;
    int draw_start = -line_height / 2 + h / 2;
    if (draw_start < 0)
      draw_start = 0;
    int draw_end = line_height / 2 + h / 2;
    if (draw_end >= h)
      draw_end = h - 1;

    // TODO: This is causing segfaults, currently patched with if statements
    // Select the texture based on the walls type (example: intersect.side could
    // be used for this)
    if (intersect.map_x < 0 || intersect.map_x > current_scene->map.width)
      intersect.map_x = 0;
    if (intersect.map_y < 0 || intersect.map_y > current_scene->map.height)
      intersect.map_y = 0;
    int tex_num = current_scene->map.walls[intersect.map_y][intersect.map_x] - 1;
    // Calculate the exact x-coordinate on the texture
    double wall_x; // Exact position where the walls was hit
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
      int tex_y = (((y * 2 - current_scene->window_ctx->window_config->height + line_height) * DEFAULT_TEXTURE_HEIGHT) /
                   line_height) /
                  2;

      // Get the color from the texture
      if (tex_num >= 0)
        color = current_scene->textures[tex_num].pixels[DEFAULT_TEXTURE_HEIGHT * tex_y + tex_x];
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
  render_copy(current_scene->window_ctx->renderer, texture, NULL, NULL);
  destroy_texture(texture);
}

void free_scene(Scene *scene)
{
  free_player(&scene->player);
  free_map(scene->map);
  free(scene);
}
