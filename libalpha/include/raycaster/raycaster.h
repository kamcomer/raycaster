#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "actor.h"
#include "camera.h"
#include "engine.h"
#include "input.h"
#include "level.h"

AeEngine *rc_engine_create(AeEngineConfig config);
void ae_engine_destroy(AeEngine *engine);

int ae_engine_load_level(AeEngine *engine, AeLevel *world);
void ae_engine_set_level(AeEngine *engine, AeLevel *world);
AeLevel *ae_engine_get_level(AeEngine *engine);

void ae_engine_set_camera(AeEngine *engine, AeCamera *cam);
AeCamera *ae_engine_get_camera(AeEngine *engine);

AeInput *ae_engine_get_input_manager(AeEngine *engine);

void ae_engine_add_actor(AeEngine *engine, RcActor *actor);
void ae_engine_remove_actor(AeEngine *engine, RcActor *actor);

void ae_engine_run(AeEngine *engine);
void ae_engine_stop(AeEngine *engine);
bool ae_engine_is_running(AeEngine *engine);

float ae_engine_get_delta_time(AeEngine *engine);

#endif // RAYCASTER_H
