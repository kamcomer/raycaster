#include "raycaster/engine.h"
#include "internal/engine_int.h"
#include "internal/level/level_int.h"
#include "internal/renderer_int.h"
#include "internal/resource/texture_int.h"
#include "kutils/str.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIXED_DT 0.0166667f

AeEngine *ae_engine_create(AeEngineConfig config)
{
  AeEngine *engine = calloc(1, sizeof(AeEngine));
  if (!engine) {
    return NULL;
  }
  engine->config = config;

  engine->running = false;

  engine->renderer = rc_renderer_create(config.rend_config);
  if (!engine->renderer) {
    ae_engine_destroy(engine);
    return NULL;
  }

  engine->input = ae_input_create(config.input_backend);
  if (!engine->input) {
    ae_engine_destroy(engine);
    return NULL;
  }

  engine->last_time = SDL_GetTicks();
  return engine;
}

void ae_engine_deinit(AeEngine *engine)
{
  if (!engine) {
    return;
  }

  if (engine->renderer) {
    ae_renderer_destroy(engine->renderer);
  }

  if (engine->level) {
    ae_level_destroy(engine->level);
  }

  if (engine->camera) {
    ae_camera_destroy(engine->camera);
  }

  if (engine->input) {
    ae_input_destroy(engine->input);
  }

  ae_texture_manager_destroy(&engine->texture_manager);
}

void ae_engine_destroy(AeEngine *engine)
{
  ae_engine_deinit(engine);
  free(engine);
}

int ae_engine_load_level(AeEngine *engine, AeLevel *level)
{
  if (!engine || !level) {
    return -1;
  }

  if (engine->level) {
    ae_level_destroy(engine->level);
  }

  engine->level = level;
  return 0;
}

AeLevel *ae_engine_get_level(AeEngine *engine) { return engine ? engine->level : NULL; }

void ae_engine_set_camera(AeEngine *engine, AeCamera *camera)
{
  if (engine) {
    engine->camera = camera;
  }
}

AeCamera *ae_engine_get_camera(AeEngine *engine) { return engine ? engine->camera : NULL; }

AeInput *ae_engine_get_input_manager(AeEngine *engine) { return engine ? engine->input : NULL; }

static void render(AeEngine *engine)
{
  if (!engine || !engine->camera || !engine->level) {
    return;
  }
  if (!engine->config.rend_config.use_gpu && !engine->renderer) {
    return;
  }

  rc_renderer_render(engine);
}

static void update(AeEngine *engine)
{
  if (!engine) {
    return;
  }

  AeCamera *camera = engine->camera;
  AeLevel *level = engine->level;

  AeInput *input_manager = engine->input;
  if (!input_manager || !camera || !level) {
    return;
  }

  float move_speed = 0.05f;
  float rot_speed = 0.03f;

  if (ae_input_get_key_down(input_manager, RC_KEY_W)) {
    double new_x = camera->pos.x + (camera->dir.x * move_speed);
    double new_y = camera->pos.y + (camera->dir.y * move_speed);
    uint32_t wall = rc_level_get_wall(level, (int)new_x, (int)new_y);
    if (wall == 0) {
      camera->pos.x = new_x;
      camera->pos.y = new_y;
    }
  }
  if (ae_input_get_key_down(input_manager, RC_KEY_S)) {
    double new_x = camera->pos.x - (camera->dir.x * move_speed);
    double new_y = camera->pos.y - (camera->dir.y * move_speed);
    uint32_t wall = rc_level_get_wall(level, (int)new_x, (int)new_y);
    if (wall == 0) {
      camera->pos.x = new_x;
      camera->pos.y = new_y;
    }
  }
  if (ae_input_get_key_down(input_manager, RC_KEY_A)) {
    double new_x = camera->pos.x - (camera->plane.x * move_speed);
    double new_y = camera->pos.y - (camera->plane.y * move_speed);
    uint32_t wall = rc_level_get_wall(level, (int)new_x, (int)new_y);
    if (wall == 0) {
      camera->pos.x = new_x;
      camera->pos.y = new_y;
    }
  }
  if (ae_input_get_key_down(input_manager, RC_KEY_D)) {
    double new_x = camera->pos.x + (camera->plane.x * move_speed);
    double new_y = camera->pos.y + (camera->plane.y * move_speed);
    uint32_t wall = rc_level_get_wall(level, (int)new_x, (int)new_y);
    if (wall == 0) {
      camera->pos.x = new_x;
      camera->pos.y = new_y;
    }
  }
  if (ae_input_get_key_down(input_manager, RC_KEY_LEFT)) {
    ae_camera_rotate(camera, -rot_speed);
  }
  if (ae_input_get_key_down(input_manager, RC_KEY_RIGHT)) {
    ae_camera_rotate(camera, rot_speed);
  }
  if (ae_input_get_key_down(input_manager, RC_KEY_ESCAPE)) {
    engine->running = false;
  }
}

static void handle_events(AeEngine *engine)
{
  if (!engine || !engine->input) {
    return;
  }

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      engine->running = false;
    }
  }
  ae_input_update(engine->input);
}

void ae_engine_run(AeEngine *engine)
{
  if (!engine) {
    return;
  }

  StringArray *texture_paths = level_get_texture_paths(engine->level);
  if (!texture_paths) {
    ae_engine_destroy(engine);
    return;
  }

  engine->texture_manager = ae_texture_load_from_file(texture_paths->items, texture_paths->len);

  engine->running = true;

  while (engine->running) {
    engine->current_time = SDL_GetTicks();
    float frame_time = (engine->current_time - engine->last_time) / 1000.0f;
    engine->last_time = engine->current_time;

    if (frame_time > 0.25f)
      frame_time = 0.25f;

    engine->accumulator += frame_time;
    engine->delta_time = frame_time;

    handle_events(engine);

    while (engine->accumulator >= FIXED_DT) {
      update(engine);
      engine->accumulator -= FIXED_DT;
    }

    render(engine);
  }
}

void ae_engine_stop(AeEngine *engine)
{
  if (engine) {
    engine->running = false;
  }
}

bool ae_engine_is_running(AeEngine *engine) { return engine ? engine->running : false; }

float ae_engine_get_delta_time(AeEngine *engine) { return engine ? engine->delta_time : 0.0f; }

AeRenderer *ae_engine_get_renderer(AeEngine *engine) { return engine ? engine->renderer : NULL; }
