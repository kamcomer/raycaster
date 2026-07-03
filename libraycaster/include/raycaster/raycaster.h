#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "actor.h"
#include "camera.h"
#include "engine.h"
#include "input.h"
#include "level.h"

typedef void (*RcUpdateFn)(void *game_state, RcEngine *engine, float delt);
typedef void (*RcRenderFn)(void *game_state, RcEngine *engine);

RcEngine *rc_engine_create(AeEngineConfig config);
void ae_engine_destroy(RcEngine *engine);

int ae_engine_load_level(RcEngine *engine, AeLevel *world);
void ae_engine_set_level(RcEngine *engine, AeLevel *world);
AeLevel *ae_engine_get_level(RcEngine *engine);

void ae_engine_set_camera(RcEngine *engine, AeCamera *cam);
AeCamera *ae_engine_get_camera(RcEngine *engine);

AeInput *ae_engine_get_input_manager(RcEngine *engine);

void rc_engine_set_update_callback(RcEngine *engine, RcUpdateFn fn, void *data);
void rc_engine_set_render_callback(RcEngine *engine, RcRenderFn fn, void *data);

void rc_engine_add_actor(RcEngine *engine, RcActor *actor);
void rc_engine_remove_actor(RcEngine *engine, RcActor *actor);

void ae_engine_run(RcEngine *engine);
void ae_engine_stop(RcEngine *engine);
bool ae_engine_is_running(RcEngine *engine);

float ae_engine_get_delta_time(RcEngine *engine);

#endif // RAYCASTER_H
