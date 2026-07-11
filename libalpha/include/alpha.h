#ifndef ALPHA_H
#define ALPHA_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef size_t uint;

typedef enum AeResult {
  A_RES_OK,
  A_RES_INVLD_ARG,
  A_RES_ALLOC_ERR,
  A_RES_BACKEND_ERR,
} AResult;

// Renderer
typedef struct ARenderer ARenderer;
typedef struct ARendererConfig ARendererConfig;

typedef enum { A_RENDERER_BACKEND_SDL } ARendererBackendType;
typedef enum { A_RENDERER_TECHNIQUE_RAYCASTER } ARendererTechniqueType;
typedef enum { A_RENDERER_SCREEN_MODE_FULLSCREEN } ARendererScreenModeType;

struct ARendererConfig {
  ARendererBackendType backend;
  ARendererScreenModeType screen_mode;
  ARendererTechniqueType technique;
  uint width;
  uint height;
  uint target_fps;
  bool show_fps;
  bool use_gpu;
  bool disable_sprites;
};

// Input
typedef struct AInput AInput;

typedef enum {
  A_KEY_UNKNOWN = 0,
  A_KEY_W,
  A_KEY_S,
  A_KEY_A,
  A_KEY_D,
  A_KEY_UP,
  A_KEY_DOWN,
  A_KEY_LEFT,
  A_KEY_RIGHT,
  A_KEY_SPACE,
  A_KEY_ESCAPE,
  A_KEY_ENTER,
  A_KEY_SHIFT,
  A_KEY_CTRL,
  A_KEY_1,
  A_KEY_2,
  A_KEY_3,
  A_KEY_4,
  A_KEY_5,
  A_KEY_6,
  A_KEY_7,
  A_KEY_8,
  A_KEY_9,
  A_KEY_0,
} AKey;

typedef enum {
  A_INPUT_BACKEND_SDL,
} AInputBackendType;

AResult a_input_create(AInputBackendType backend, AInput **out);
AResult a_input_init(AInputBackendType backend, AInput *out);
void a_input_deinit(AInputBackendType backend, AInput *out);
void a_input_destroy(AInput *input);

void a_input_update(AInput *input);
bool a_input_get_key_down(AInput *input, AKey key);
bool a_input_get_key_pressed(AInput *input, AKey key);

// Level
typedef struct ALevel ALevel;
typedef struct ASprite ASprite;

ALevel *a_level_load_from_file(const char *file_path);
AResult a_level_create(ALevel **out);
AResult a_level_init(ALevel *out);
void a_level_deinit(ALevel *out);
void a_level_destroy(ALevel *level);

uint32_t a_level_get_width(ALevel *level);
uint32_t a_level_get_height(ALevel *level);
uint32_t a_level_get_wall(ALevel *level, int idx_x, int idx_y);
uint32_t a_level_get_floor(ALevel *level, int idx_x, int idx_y);
uint32_t a_level_get_ceil(ALevel *level, int idx_x, int idx_y);
uint32_t a_level_get_unit_size(ALevel *level);
void a_level_get_sprites(ALevel *level, ASprite **out, uint *count);
void a_level_update(ALevel *level, float delta_t);

// Engine
typedef struct AEngine AEngine;
typedef struct ACamera ACamera;
typedef struct AActor AActor;

typedef struct {
  ARendererConfig rend_config;
  AInputBackendType input_backend;
} AEngineConfig;

AResult a_engine_create(AEngineConfig config, AEngine **out);
void a_engine_destroy(AEngine *engine);

int a_engine_load_level(AEngine *engine, ALevel *level);
void a_engine_set_level(AEngine *engine, ALevel *level);
ALevel *a_engine_get_level(AEngine *engine);

void a_engine_set_camera(AEngine *engine, ACamera *cam);
ACamera *a_engine_get_camera(AEngine *engine);

AInput *a_engine_get_input_manager(AEngine *engine);

void a_engine_add_actor(AEngine *engine, AActor *actor);
void a_engine_remove_actor(AEngine *engine, AActor *actor);

void a_engine_run(AEngine *engine);
void a_engine_stop(AEngine *engine);
bool a_engine_is_running(AEngine *engine);

float a_engine_get_delta_time(AEngine *engine);

#endif // ALPHA_H
