#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "actor.h"
#include "camera.h"
#include "engine.h"
#include "input.h"
#include "level.h"
#include "renderer.h"
#include "types.h"

typedef void (*RcUpdateFn)(void *game_state, RcEngine *engine, float dt);
typedef void (*RcRenderFn)(void *game_state, RcEngine *engine);

RcEngine *rc_engine_create(RcEngineConfig config);
void rc_engine_destroy(RcEngine *e);

int rc_engine_load_level(RcEngine *e, RcLevel *world);
void rc_engine_set_level(RcEngine *e, RcLevel *world);
RcLevel *rc_engine_get_level(RcEngine *e);

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

void rc_engine_config_set_defaults(RcEngineConfig *cfg);

#endif // RAYCASTER_H
