#include "alpha.h"
#include "internal/i_alpha.h"
#include "kutils/str.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIXED_DT 0.0166667f

AResult a_engine_create(AEngineConfig config, AEngine **out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = NULL;

  AEngine *engine = malloc(sizeof(AEngine));
  if (!engine) {
    return A_RES_ALLOC_ERR;
  }

  engine->running = false;
  engine->config = config;

  // TODO Remove this, it is a bandaid
  engine->scene = malloc(sizeof(AScene));

  const float cam_pos = 3.5;

  AResult res =
      a_camera_create(config.rend_config.width, config.rend_config.height, &engine->scene->camera);
  if (res != A_RES_OK) {
    a_engine_destroy(engine);
    return res;
  }
  a_camera_set_position(engine->scene->camera, cam_pos, cam_pos);
  a_camera_set_direction(engine->scene->camera, -1.0, 0.0);

  res = a_renderer_create(engine->scene, config.rend_config, &engine->renderer);
  if (res != A_RES_OK) {
    a_engine_destroy(engine);
    return res;
  }

  res = a_input_create(config.input_backend, &engine->input);
  if (res != A_RES_OK) {
    a_engine_destroy(engine);
    return res;
  }

  engine->last_time = SDL_GetTicks();
  *out = engine;
  return A_RES_OK;
}

void a_engine_deinit(AEngine *engine)
{
  if (!engine) {
    return;
  }

  if (engine->renderer) {
    a_renderer_destroy(engine->renderer);
  }

  if (engine->level) {
    a_level_destroy(engine->level);
  }

  if (engine->input) {
    a_input_destroy(engine->input);
  }

  /* a_texture_manager_destroy(&engine->texture_manager); */
}

void a_engine_destroy(AEngine *engine)
{
  a_engine_deinit(engine);
  free(engine);
}

int a_engine_load_level(AEngine *engine, ALevel *level)
{
  if (!engine || !level) {
    return -1;
  }

  if (engine->level) {
    a_level_destroy(engine->level);
  }

  engine->level = level;
  return 0;
}

ALevel *a_engine_get_level(AEngine *engine) { return engine ? engine->level : NULL; }

AInput *a_engine_get_input_manager(AEngine *engine) { return engine ? engine->input : NULL; }

static void render(AEngine *engine)
{
  if (!engine || !engine->level) {
    return;
  }
  if (!engine->config.rend_config.use_gpu && !engine->renderer) {
    return;
  }

  a_renderer_technique_process(&engine->renderer->technique);
  a_renderer_backend_render(&engine->renderer->backend);
}

static void update(AEngine *engine)
{
  if (!engine) {
    return;
  }

  ACamera *camera = engine->scene->camera;
  ALevel *level = engine->level;

  AInput *input_manager = engine->input;
  if (!input_manager || !camera || !level) {
    return;
  }

  float move_speed = 0.05f;
  float rot_speed = 0.03f;

  if (a_input_get_key_down(input_manager, A_KEY_W)) {
    double new_x = camera->pos.x + (camera->dir.x * move_speed);
    double new_y = camera->pos.y + (camera->dir.y * move_speed);
    uint32_t wall = a_level_get_wall(level, (int)new_x, (int)new_y);
    if (wall == 0) {
      camera->pos.x = new_x;
      camera->pos.y = new_y;
    }
  }
  if (a_input_get_key_down(input_manager, A_KEY_S)) {
    double new_x = camera->pos.x - (camera->dir.x * move_speed);
    double new_y = camera->pos.y - (camera->dir.y * move_speed);
    uint32_t wall = a_level_get_wall(level, (int)new_x, (int)new_y);
    if (wall == 0) {
      camera->pos.x = new_x;
      camera->pos.y = new_y;
    }
  }
  if (a_input_get_key_down(input_manager, A_KEY_A)) {
    double new_x = camera->pos.x - (camera->plane.x * move_speed);
    double new_y = camera->pos.y - (camera->plane.y * move_speed);
    uint32_t wall = a_level_get_wall(level, (int)new_x, (int)new_y);
    if (wall == 0) {
      camera->pos.x = new_x;
      camera->pos.y = new_y;
    }
  }
  if (a_input_get_key_down(input_manager, A_KEY_D)) {
    double new_x = camera->pos.x + (camera->plane.x * move_speed);
    double new_y = camera->pos.y + (camera->plane.y * move_speed);
    uint32_t wall = a_level_get_wall(level, (int)new_x, (int)new_y);
    if (wall == 0) {
      camera->pos.x = new_x;
      camera->pos.y = new_y;
    }
  }
  if (a_input_get_key_down(input_manager, A_KEY_LEFT)) {
    a_camera_rotate(camera, -rot_speed);
  }
  if (a_input_get_key_down(input_manager, A_KEY_RIGHT)) {
    a_camera_rotate(camera, rot_speed);
  }
  if (a_input_get_key_down(input_manager, A_KEY_ESCAPE)) {
    engine->running = false;
  }
}

static void handle_events(AEngine *engine)
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
  a_input_update(engine->input);
}

void a_engine_run(AEngine *engine)
{
  if (!engine) {
    return;
  }

  // TODO: this is hackey
  StringArray *texture_paths = a_level_get_texture_paths(engine->level);
  if (!texture_paths) {
    a_engine_destroy(engine);
    return;
  }

  ATextureArray *texture_array;
  a_texture_array_create(texture_paths->len, &texture_array);
  for (uint i = 0; i < texture_paths->len; i++) {
    a_texture_load_from_file(texture_paths->items[i], &(texture_array->items[i]));
  }

  engine->scene->textures = texture_array;
  engine->scene->level = engine->level;

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

void a_engine_stop(AEngine *engine)
{
  if (engine) {
    engine->running = false;
  }
}

bool a_engine_is_running(AEngine *engine) { return engine ? engine->running : false; }

float a_engine_get_delta_time(AEngine *engine) { return engine ? engine->delta_time : 0.0f; }

ARenderer *a_engine_get_renderer(AEngine *engine) { return engine ? engine->renderer : NULL; }
