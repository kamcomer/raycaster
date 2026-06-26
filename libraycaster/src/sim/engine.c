#include "raycaster/engine.h"
#include "internal/engine_int.h"
#include "internal/renderer_int.h"
#include "internal/asset/texture_int.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIXED_DT 0.0166667f

void rc_engine_config_set_defaults(RcEngineConfig *cfg)
{
  if (!cfg)
    return;
  cfg->map_file = NULL;
  cfg->strip_count = 4;
}

RcEngine *rc_engine_create(RcEngineConfig config)
{
  RcEngine *e = calloc(1, sizeof(RcEngine));
  if (!e) {
    fprintf(stderr, "Failed to allocate RcEngine\n");
    SDL_Quit();
    return NULL;
  }

  e->config = config;
  e->running = false;

  e->renderer = rc_renderer_create(config.rend_config);
  if (!e->renderer) {
    fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
    free(e);
    SDL_Quit();
    return NULL;
  }

  e->input = rc_input_create(config.input_backend);
  if (!e->input) {
    return NULL;
  }

  e->last_time = SDL_GetTicks();
  e->accumulator = 0.0f;

  e->textures = rc_create_textures();

  return e;
}

void rc_engine_destroy(RcEngine *e)
{
  if (!e)
    return;

  if (e->renderer)
    rc_renderer_destroy(e->renderer);

  if (e->level)
    rc_level_destroy(e->level);

  if (e->camera)
    rc_camera_destroy(e->camera);

  if (e->input)
    rc_input_destroy(e->input);

  if (e->textures)
    rc_free_textures(e->textures);

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

static void handle_events(RcEngine *e);
static void update(RcEngine *e);
static void render(RcEngine *e);

static void render(RcEngine *e)
{
  if (!e || !e->camera || !e->level)
    return;
  if (!e->config.rend_config.use_gpu && !e->renderer)
    return;

  rc_renderer_render(e);

  if (e->render_fn && e->game_state)
    e->render_fn(e->game_state, e);
}

static void update(RcEngine *e)
{
  if (!e)
    return;

  if (e->update_fn && e->game_state) {
    e->update_fn(e->game_state, e, e->delta_time);
  }

  RcCamera *cam = e->camera;
  RcLevel *world = e->level;

  RcInput *in = e->input;
  if (!in || !cam || !world)
    return;

  float move_speed = 0.05f;
  float rot_speed = 0.03f;

  if (rc_input_get_key_down(in, RC_KEY_W)) {
    double new_x = cam->pos.x + cam->dir.x * move_speed;
    double new_y = cam->pos.y + cam->dir.y * move_speed;
    uint32_t wall = rc_level_get_wall(world, (int)new_x, (int)new_y);
    if (wall == 0) {
      cam->pos.x = new_x;
      cam->pos.y = new_y;
    }
  }
  if (rc_input_get_key_down(in, RC_KEY_S)) {
    double new_x = cam->pos.x - cam->dir.x * move_speed;
    double new_y = cam->pos.y - cam->dir.y * move_speed;
    uint32_t wall = rc_level_get_wall(world, (int)new_x, (int)new_y);
    if (wall == 0) {
      cam->pos.x = new_x;
      cam->pos.y = new_y;
    }
  }
  if (rc_input_get_key_down(in, RC_KEY_A)) {
    double new_x = cam->pos.x - cam->plane.x * move_speed;
    double new_y = cam->pos.y - cam->plane.y * move_speed;
    uint32_t wall = rc_level_get_wall(world, (int)new_x, (int)new_y);
    if (wall == 0) {
      cam->pos.x = new_x;
      cam->pos.y = new_y;
    }
  }
  if (rc_input_get_key_down(in, RC_KEY_D)) {
    double new_x = cam->pos.x + cam->plane.x * move_speed;
    double new_y = cam->pos.y + cam->plane.y * move_speed;
    uint32_t wall = rc_level_get_wall(world, (int)new_x, (int)new_y);
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

    if (e->config.rend_config.target_fps > 0) {
      int work_ms = SDL_GetTicks() - e->current_time;
      int target_ms = (int)(1000.0f / e->config.rend_config.target_fps + 0.5f);
      int delay = target_ms - work_ms;
      if (delay > 0)
        SDL_Delay(delay);
    }

    e->frame_count++;
    e->fps_timer += SDL_GetTicks() - e->current_time;
    if (e->fps_timer >= 1000) {
      e->fps = e->frame_count * 1000.0f / e->fps_timer;
      e->frame_count = 0;
      e->fps_timer = 0;

      // if (e->config.rend_config.show_fps && rc_renderer_get_window(e->renderer)) {
      //   char title[256];
      //   snprintf(title, sizeof(title), "%s - FPS: %.1f",
      //            e->config.rend_config.title ? e->config.rend_config.title : "Raycaster",
      //            e->fps);
      //   // SDL_SetWindowTitle(e->window, title);
      // }
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

RcRenderer *rc_engine_get_renderer(RcEngine *e) { return e ? e->renderer : NULL; }
