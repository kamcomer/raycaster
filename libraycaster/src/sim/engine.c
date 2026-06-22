#include "raycaster/raycaster.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIXED_DT 0.0166667f

struct RcEngine {
  RcConfig config;
  RcLevel *level;
  RcCamera *camera;
  RcInput *input;
  void *game_state;
  RcUpdateFn update_fn;
  RcRenderFn render_fn;
  bool running;
  float delta_time;

  SDL_Window *window;
  SDL_Renderer *renderer;
  uint32_t last_time;
  uint32_t current_time;
  float accumulator;

  int frame_count;
  float fps;
  uint32_t fps_timer;

  RcTextureData *textures;

  SDL_Texture *wall_texture;
  SDL_Texture *wall_textures[11];
  SDL_Texture *floor_ceil_texture;
  SDL_Texture *sprite_texture;
  SDL_Texture *floor_texture;
  SDL_Texture *ceiling_texture;
  SDL_Texture *sprite_textures[3];
};

void rc_config_set_defaults(RcConfig *cfg)
{
  if (!cfg)
    return;
  cfg->title = "Raycaster Game";
  cfg->width = 800;
  cfg->height = 600;
  cfg->target_fps = 60;
  cfg->show_fps = false;
  cfg->map_file = NULL;
  cfg->strip_count = 4;
}

static int init_sdl(void)
{
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return -1;
  }

  return 0;
}

static void shutdown_sdl(void) { SDL_Quit(); }

static uint32_t *load_image_data(const char *file_path, int *ptr_width, int *ptr_height)
{
  SDL_Surface *texture_surface = IMG_Load(file_path);
  if (!texture_surface) {
    fprintf(stderr, "Could not load image: %s\n", SDL_GetError());
    return NULL;
  }

  *ptr_width = texture_surface->w;
  *ptr_height = texture_surface->h;

  SDL_Surface *formatted_surface = SDL_ConvertSurface(texture_surface, SDL_PIXELFORMAT_RGBA8888);

  SDL_DestroySurface(texture_surface);
  if (!formatted_surface) {
    fprintf(stderr, "Could not convert surface to RGBA8888: %s\n", SDL_GetError());
    return NULL;
  }

  uint32_t *texture_pixels = malloc(formatted_surface->w * formatted_surface->h * sizeof(uint32_t));
  if (!texture_pixels) {
    fprintf(stderr, "Could not allocate memory for pixel data\n");
    SDL_DestroySurface(formatted_surface);
    return NULL;
  }

  uint32_t *formatted_pixels = (uint32_t *)formatted_surface->pixels;
  for (int current_row = 0; current_row < formatted_surface->h; current_row++) {
    for (int current_col = 0; current_col < formatted_surface->w; current_col++) {
      uint32_t pixel = formatted_pixels[current_row * formatted_surface->w + current_col];
      texture_pixels[current_row * formatted_surface->w + current_col] = pixel;
    }
  }

  SDL_DestroySurface(formatted_surface);

  return texture_pixels;
}

static RcTextureData *create_textures(void)
{
  RcTextureData *textures = malloc(sizeof(RcTextureData) * 11);
  if (!textures)
    return NULL;

  int width, height;

  uint32_t *t0 = load_image_data("assets/textures/bluestone.png", &width, &height);
  uint32_t *t1 = load_image_data("assets/textures/colorstone.png", &width, &height);
  uint32_t *t2 = load_image_data("assets/textures/eagle.png", &width, &height);
  uint32_t *t3 = load_image_data("assets/textures/greystone.png", &width, &height);
  uint32_t *t4 = load_image_data("assets/textures/mossy.png", &width, &height);
  uint32_t *t5 = load_image_data("assets/textures/purplestone.png", &width, &height);
  uint32_t *t6 = load_image_data("assets/textures/redbrick.png", &width, &height);
  uint32_t *t7 = load_image_data("assets/textures/wood.png", &width, &height);
  uint32_t *t8 = load_image_data("assets/textures/barrel.png", &width, &height);
  uint32_t *t9 = load_image_data("assets/textures/pillar.png", &width, &height);
  uint32_t *t10 = load_image_data("assets/textures/greenlight.png", &width, &height);

  for (int i = 0; i < 11; i++) {
    textures[i].pixels = malloc(sizeof(uint32_t) * (RC_TEXTURE_WIDTH * RC_TEXTURE_HEIGHT));
    textures[i].width = RC_TEXTURE_WIDTH;
    textures[i].height = RC_TEXTURE_HEIGHT;
  }

  uint32_t *sources[] = {t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10};

  for (int i = 0; i < 11; i++) {
    if (sources[i]) {
      for (int j = 0; j < RC_TEXTURE_WIDTH * RC_TEXTURE_HEIGHT && j < width * height; j++) {
        textures[i].pixels[j] = sources[i][j];
      }
      free(sources[i]);
    }
  }

  return textures;
}

static void free_textures(RcTextureData *textures)
{
  if (!textures)
    return;
  for (int i = 0; i < 11; i++) {
    free(textures[i].pixels);
  }
  free(textures);
}

RcEngine *rc_engine_create(RcConfig config)
{
  if (init_sdl() != 0)
    return NULL;

  RcEngine *e = calloc(1, sizeof(RcEngine));
  if (!e) {
    fprintf(stderr, "Failed to allocate RcEngine\n");
    shutdown_sdl();
    return NULL;
  }

  e->config = config;
  e->running = false;
  e->window =
      SDL_CreateWindow(config.title ? config.title : "Raycaster", config.width, config.height, 0);

  if (!e->window) {
    fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
    free(e);
    shutdown_sdl();
    return NULL;
  }

  SDL_SetWindowPosition(e->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  e->renderer = SDL_CreateRenderer(e->window, NULL);
  if (!e->renderer) {
    fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(e->window);
    free(e);
    shutdown_sdl();
    return NULL;
  }

  e->camera = rc_camera_create(config.width, config.height);
  rc_camera_set_position(e->camera, 3.5, 3.5);
  rc_camera_set_direction(e->camera, -1.0, 0.0);

  e->input = rc_input_create(config.input_backend);

  e->textures = create_textures();

  if (!e->textures) {
    fprintf(stderr, "Failed to create textures\n");
    rc_camera_destroy(e->camera);
    SDL_DestroyRenderer(e->renderer);
    SDL_DestroyWindow(e->window);
    free(e);
    shutdown_sdl();
    return NULL;
  }

  for (int i = 0; i < 11; i++) {
    e->wall_textures[i] =
        SDL_CreateTexture(e->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC,
                          RC_TEXTURE_WIDTH, RC_TEXTURE_HEIGHT);
    if (!e->wall_textures[i]) {
      fprintf(stderr, "Failed to create wall texture %d\n", i);
    } else {
      SDL_SetTextureBlendMode(e->wall_textures[i], SDL_BLENDMODE_NONE);
      SDL_UpdateTexture(e->wall_textures[i], NULL, e->textures[i].pixels, RC_TEXTURE_WIDTH * 4);
    }
  }

  e->wall_texture = SDL_CreateTexture(e->renderer, SDL_PIXELFORMAT_RGBA8888,
                                      SDL_TEXTUREACCESS_STREAMING, config.width, config.height);
  if (e->wall_texture) {
    SDL_SetTextureBlendMode(e->wall_texture, SDL_BLENDMODE_BLEND);
  }

  e->floor_ceil_texture =
      SDL_CreateTexture(e->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                        config.width, config.height);
  if (e->floor_ceil_texture) {
    SDL_SetTextureBlendMode(e->floor_ceil_texture, SDL_BLENDMODE_BLEND);
  }

  e->sprite_texture = SDL_CreateTexture(e->renderer, SDL_PIXELFORMAT_RGBA8888,
                                        SDL_TEXTUREACCESS_STREAMING, config.width, config.height);
  if (e->sprite_texture) {
    SDL_SetTextureBlendMode(e->sprite_texture, SDL_BLENDMODE_BLEND);
  }

  int floor_tex = 7;
  e->floor_texture =
      SDL_CreateTexture(e->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC,
                        RC_TEXTURE_WIDTH, RC_TEXTURE_HEIGHT);
  if (e->floor_texture) {
    SDL_SetTextureBlendMode(e->floor_texture, SDL_BLENDMODE_NONE);
    SDL_UpdateTexture(e->floor_texture, NULL, e->textures[floor_tex].pixels, RC_TEXTURE_WIDTH * 4);
  }

  int ceil_tex = 3;
  e->ceiling_texture =
      SDL_CreateTexture(e->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC,
                        RC_TEXTURE_WIDTH, RC_TEXTURE_HEIGHT);
  if (e->ceiling_texture) {
    SDL_SetTextureBlendMode(e->ceiling_texture, SDL_BLENDMODE_NONE);
    SDL_UpdateTexture(e->ceiling_texture, NULL, e->textures[ceil_tex].pixels, RC_TEXTURE_WIDTH * 4);
  }

  for (int i = 0; i < 3; i++) {
    int tex_num = 8 + i;
    e->sprite_textures[i] =
        SDL_CreateTexture(e->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC,
                          RC_TEXTURE_WIDTH, RC_TEXTURE_HEIGHT);
    if (!e->sprite_textures[i]) {
      fprintf(stderr, "Failed to create sprite texture %d\n", i);
    } else {
      SDL_SetTextureBlendMode(e->sprite_textures[i], SDL_BLENDMODE_BLEND);
      SDL_UpdateTexture(e->sprite_textures[i], NULL, e->textures[tex_num].pixels,
                        RC_TEXTURE_WIDTH * 4);
    }
  }

  e->last_time = SDL_GetTicks();
  e->accumulator = 0.0f;

  return e;
}

void rc_engine_destroy(RcEngine *e)
{
  if (!e)
    return;

  if (e->window)
    SDL_DestroyWindow(e->window);

  if (e->renderer)
    SDL_DestroyRenderer(e->renderer);

  if (e->level)
    rc_level_destroy(e->level);

  if (e->camera)
    rc_camera_destroy(e->camera);

  if (e->input)
    rc_input_destroy(e->input);

  if (e->textures)
    free_textures(e->textures);

  if (e->wall_texture)
    SDL_DestroyTexture(e->wall_texture);
  for (int i = 0; i < 11; i++) {
    if (e->wall_textures[i])
      SDL_DestroyTexture(e->wall_textures[i]);
  }
  if (e->floor_ceil_texture)
    SDL_DestroyTexture(e->floor_ceil_texture);
  if (e->sprite_texture)
    SDL_DestroyTexture(e->sprite_texture);
  if (e->floor_texture)
    SDL_DestroyTexture(e->floor_texture);
  if (e->ceiling_texture)
    SDL_DestroyTexture(e->ceiling_texture);
  for (int i = 0; i < 3; i++) {
    if (e->sprite_textures[i])
      SDL_DestroyTexture(e->sprite_textures[i]);
  }

  shutdown_sdl();
  free(e);
}

int rc_engine_load_level(RcEngine *e, RcLevel *world)
{
  if (!e || !world)
    return -1;

  if (e->level)
    rc_level_destroy(e->level);

  e->level = world;
  return 0;
}

void rc_engine_set_level(RcEngine *e, RcLevel *world)
{
  if (e)
    e->level = world;
}

RcLevel *rc_engine_get_level(RcEngine *e) { return e ? e->level : NULL; }

void rc_engine_set_camera(RcEngine *e, RcCamera *cam)
{
  if (e)
    e->camera = cam;
}

RcCamera *rc_engine_get_camera(RcEngine *e) { return e ? e->camera : NULL; }

RcInput *rc_engine_get_input(RcEngine *e) { return e ? e->input : NULL; }

void rc_engine_set_update_callback(RcEngine *e, RcUpdateFn fn, void *data)
{
  if (e) {
    e->update_fn = fn;
    e->game_state = data;
  }
}

void rc_engine_set_render_callback(RcEngine *e, RcRenderFn fn, void *data)
{
  if (e) {
    e->render_fn = fn;
    e->game_state = data;
  }
}

void rc_engine_add_actor(RcEngine *e, RcActor *actor)
{
  (void)e;
  (void)actor;
}

void rc_engine_remove_actor(RcEngine *e, RcActor *actor)
{
  (void)e;
  (void)actor;
}

static void render_walls(RcEngine *e, double *z_buffer);
static void render_floor_ceiling(RcEngine *e);
static void render_sprites(RcEngine *e, double *z_buffer);
static void handle_events(RcEngine *e);
static void update(RcEngine *e);
static void render(RcEngine *e);

static void render(RcEngine *e)
{
  if (!e || !e->renderer || !e->camera || !e->level)
    return;

  int w = e->config.width;
  double *z_buffer = malloc(sizeof(double) * w);

  SDL_SetRenderDrawColor(e->renderer, 30, 30, 50, 255);
  SDL_RenderClear(e->renderer);

  render_floor_ceiling(e);
  render_walls(e, z_buffer);
  render_sprites(e, z_buffer);

  free(z_buffer);

  if (e->render_fn && e->game_state) {
    e->render_fn(e->game_state, e);
  }

  SDL_RenderPresent(e->renderer);
}

static void render_floor_ceiling(RcEngine *e)
{
  RcCamera *cam = e->camera;
  SDL_Renderer *r = e->renderer;
  int w = e->config.width;
  int h = e->config.height;

  SDL_Texture *texture = e->floor_ceil_texture;
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  void *pixels;
  int pitch;
  SDL_LockTexture(texture, NULL, &pixels, &pitch);
  memset(pixels, 0, pitch * h);
  uint32_t *pixel_data = (uint32_t *)pixels;

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
        pixel_data[(y * (pitch / 4)) + x] = floor_color;

        uint32_t ceil_color = e->textures[ceil_tex].pixels[RC_TEXTURE_HEIGHT * ty + tx];
        pixel_data[((h - y) * (pitch / 4)) + x] = ceil_color;
      } else {
        pixel_data[(y * (pitch / 4)) + x] = 0xFF606078;
        pixel_data[((h - y) * (pitch / 4)) + x] = 0xFF282840;
      }
    }
  }

  SDL_UnlockTexture(texture);
  SDL_RenderTexture(r, texture, NULL, NULL);
}

static void render_sprites(RcEngine *e, double *z_buffer)
{
  RcCamera *cam = e->camera;
  RcLevel *world = e->level;
  SDL_Renderer *r = e->renderer;
  int w = e->config.width;
  int h = e->config.height;

  RcSprite *sprites = NULL;
  int sprite_count = 0;
  rc_level_get_sprites(world, &sprites, &sprite_count);

  if (sprite_count == 0 || !sprites)
    return;

  double *sprite_dist = malloc(sizeof(double) * sprite_count);
  int *sprite_order = malloc(sizeof(int) * sprite_count);

  for (int i = 0; i < sprite_count; i++) {
    sprite_order[i] = i;
    sprite_dist[i] = ((cam->pos.x - sprites[i].pos.x) * (cam->pos.x - sprites[i].pos.x) +
                      (cam->pos.y - sprites[i].pos.y) * (cam->pos.y - sprites[i].pos.y));
  }

  for (int i = 0; i < sprite_count - 1; i++) {
    for (int j = i + 1; j < sprite_count; j++) {
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

  SDL_Texture *texture = e->sprite_texture;
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  void *pixels;
  int pitch;
  SDL_LockTexture(texture, NULL, &pixels, &pitch);
  memset(pixels, 0, pitch * h);
  uint32_t *pixel_data = (uint32_t *)pixels;

  for (int i = 0; i < sprite_count; i++) {
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
            pixel_data[(y * (pitch / 4)) + stripe] = color;
          }
        }
      }
    }
  }

  SDL_UnlockTexture(texture);
  SDL_RenderTexture(r, texture, NULL, NULL);

  free(sprite_dist);
  free(sprite_order);
}

static void render_walls(RcEngine *e, double *z_buffer)
{
  RcCamera *cam = e->camera;
  RcLevel *world = e->level;
  SDL_Renderer *r = e->renderer;
  int w = e->config.width;
  int h = e->config.height;

  SDL_Texture *texture = e->wall_texture;
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  void *pixels;
  int pitch;
  SDL_LockTexture(texture, NULL, &pixels, &pitch);
  memset(pixels, 0, pitch * h);
  uint32_t *pixel_data = (uint32_t *)pixels;

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

    int wall_type = rc_level_get_wall(world, map_x, map_y);
    int tex_num = wall_type - 1;

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

      pixel_data[(y * (pitch / 4)) + x] = color;
    }
  }

  SDL_UnlockTexture(texture);
  SDL_RenderTexture(r, texture, NULL, NULL);
}

static void update(RcEngine *e)
{
  if (!e)
    return;

  if (e->update_fn && e->game_state) {
    e->update_fn(e->game_state, e, e->delta_time);
  }

  RcInput *in = e->input;
  RcCamera *cam = e->camera;
  RcLevel *world = e->level;

  if (!in || !cam || !world)
    return;

  float move_speed = 0.05f;
  float rot_speed = 0.03f;

  if (rc_input_get_key_down(in, RC_KEY_W)) {
    double new_x = cam->pos.x + cam->dir.x * move_speed;
    double new_y = cam->pos.y + cam->dir.y * move_speed;
    int wall = rc_level_get_wall(world, (int)new_x, (int)new_y);
    if (wall == 0) {
      cam->pos.x = new_x;
      cam->pos.y = new_y;
    }
  }
  if (rc_input_get_key_down(in, RC_KEY_S)) {
    double new_x = cam->pos.x - cam->dir.x * move_speed;
    double new_y = cam->pos.y - cam->dir.y * move_speed;
    int wall = rc_level_get_wall(world, (int)new_x, (int)new_y);
    if (wall == 0) {
      cam->pos.x = new_x;
      cam->pos.y = new_y;
    }
  }
  if (rc_input_get_key_down(in, RC_KEY_A)) {
    double new_x = cam->pos.x - cam->plane.x * move_speed;
    double new_y = cam->pos.y - cam->plane.y * move_speed;
    int wall = rc_level_get_wall(world, (int)new_x, (int)new_y);
    if (wall == 0) {
      cam->pos.x = new_x;
      cam->pos.y = new_y;
    }
  }
  if (rc_input_get_key_down(in, RC_KEY_D)) {
    double new_x = cam->pos.x + cam->plane.x * move_speed;
    double new_y = cam->pos.y + cam->plane.y * move_speed;
    int wall = rc_level_get_wall(world, (int)new_x, (int)new_y);
    if (wall == 0) {
      cam->pos.x = new_x;
      cam->pos.y = new_y;
    }
  }
  if (rc_input_get_key_down(in, RC_KEY_LEFT)) {
    rc_camera_rotate(cam, -rot_speed);
  }
  if (rc_input_get_key_down(in, RC_KEY_RIGHT)) {
    rc_camera_rotate(cam, rot_speed);
  }
  if (rc_input_get_key_down(in, RC_KEY_ESCAPE)) {
    e->running = false;
  }
}

static void handle_events(RcEngine *e)
{
  if (!e || !e->input)
    return;

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      e->running = false;
    }
  }
  rc_input_update(e->input);
}

void rc_engine_run(RcEngine *e)
{
  if (!e)
    return;

  e->running = true;

  while (e->running) {
    e->current_time = SDL_GetTicks();
    float frame_time = (e->current_time - e->last_time) / 1000.0f;
    e->last_time = e->current_time;

    if (frame_time > 0.25f)
      frame_time = 0.25f;

    e->accumulator += frame_time;
    e->delta_time = frame_time;

    handle_events(e);

    while (e->accumulator >= FIXED_DT) {
      update(e);
      e->accumulator -= FIXED_DT;
    }

    render(e);

    e->frame_count++;
    e->fps_timer += (uint32_t)(frame_time * 1000);
    if (e->fps_timer >= 1000) {
      e->fps = e->frame_count * 1000.0f / e->fps_timer;
      e->frame_count = 0;
      e->fps_timer = 0;

      if (e->config.show_fps && e->window) {
        char title[256];
        snprintf(title, sizeof(title), "%s - FPS: %.1f",
                 e->config.title ? e->config.title : "Raycaster", e->fps);
        SDL_SetWindowTitle(e->window, title);
      }
    }

    if (e->config.target_fps > 0) {
      int delay = 1000 / e->config.target_fps - (int)(frame_time * 1000);
      if (delay > 0)
        SDL_Delay(delay);
    }
  }
}

void rc_engine_stop(RcEngine *e)
{
  if (e)
    e->running = false;
}

bool rc_engine_is_running(RcEngine *e) { return e ? e->running : false; }

float rc_engine_get_delta_time(RcEngine *e) { return e ? e->delta_time : 0.0f; }
