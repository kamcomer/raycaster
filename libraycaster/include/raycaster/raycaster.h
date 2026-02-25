#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "types.h"
#include "input.h"
#include "camera.h"
#include "world.h"
#include "actor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RcEngine RcEngine;

typedef struct RcConfig
{
    const char *title;
    int width;
    int height;
    int target_fps;
    bool show_fps;
    const char *map_file;
    int strip_count;
} RcConfig;

typedef struct RcLevel
{
    char name[64];
    RcVector player_start;
    char map_file[256];
} RcLevel;

typedef void (*RcUpdateFn)(void *game_state, RcEngine *engine, float dt);
typedef void (*RcRenderFn)(void *game_state, RcEngine *engine);

RcEngine *rc_engine_create(RcConfig config);
void rc_engine_destroy(RcEngine *e);

int rc_engine_load_world(RcEngine *e, RcWorld *world);
void rc_engine_set_world(RcEngine *e, RcWorld *world);
RcWorld *rc_engine_get_world(RcEngine *e);

void rc_engine_set_camera(RcEngine *e, RcCamera *cam);
RcCamera *rc_engine_get_camera(RcEngine *e);

RcInput *rc_engine_get_input(RcEngine *e);

void rc_engine_set_update_callback(RcEngine *e, RcUpdateFn fn, void *data);
void rc_engine_set_render_callback(RcEngine *e, RcRenderFn fn, void *data);

void rc_engine_add_actor(RcEngine *e, RcActor *actor);
void rc_engine_remove_actor(RcEngine *e, RcActor *actor);

void rc_engine_run(RcEngine *e);
void rc_engine_stop(RcEngine *e);
bool rc_engine_is_running(RcEngine *e);

float rc_engine_get_delta_time(RcEngine *e);

RcWorld *rc_world_load_from_file(const char *map_path);
RcWorld *rc_world_create_empty(int width, int height);

void rc_config_set_defaults(RcConfig *cfg);

#ifdef __cplusplus
}
#endif

#endif // RAYCASTER_H
