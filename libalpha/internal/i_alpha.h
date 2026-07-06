#ifndef I_ALPHA_H
#define I_ALPHA_H

#include "alpha.h"
#include "kutils/str.h"
#include <stdbool.h>
#include <stdlib.h>

// Types
#define A_PI 3.14159265358979323846
#define A_PI_2 (A_PI / 2.0)
#define A_DEG_TO_RAD (A_PI / 180.0)

#define A_TEXTURE_WIDTH 64
#define A_TEXTURE_HEIGHT 64

typedef size_t uint;

typedef struct {
  double x;
  double y;
  double mag;
  double angle;
} AVector;

typedef struct {
  int x;
  int y;
  uint w;
  uint h;
} ARect;

typedef struct {
  uint width;
  uint height;
} ADimensions;

// Texture Manager
typedef size_t ATextureHandle;

typedef struct {
  uint32_t *pixels;
  uint height;
  uint width;
} ATexture;

typedef struct {
  ATexture *items;
  uint len;
  uint capacity;
} ATextureArray;

typedef struct {
  ATextureArray cache;
} ATextureManager;

AResult a_texture_load_from_file(const char *file_path, ATexture **out);

AResult a_texture_manager_create(size_t cache_size, ATextureManager **out);
AResult a_texture_array_create(size_t size, ATextureArray **out);
AResult a_texture_create(const uint32_t *pixels, size_t width, size_t height, ATexture **out);

AResult a_texture_manager_init(size_t cache_size, ATextureManager *out);
AResult a_texture_array_init(size_t size, ATextureArray *out);
AResult a_texture_init(const uint32_t *pixels, size_t width, size_t height, ATexture *out);

void a_texture_manager_destroy(ATextureManager *manager);
void a_texture_array_destroy(ATextureArray *array);
void a_texture_destroy(ATexture *texture);

void a_texture_manager_deinit(ATextureManager *manager);
void a_texture_array_deinit(ATextureArray *array);
void a_texture_deinit(ATexture *texture);

// Material
typedef struct {
  ATextureHandle texture;
} AMaterial;

typedef struct {
  AMaterial *items;
  uint len;
  uint capacity;
} AMaterialArray;

// Asset Manager
typedef struct {
  ATextureManager texture_manager;
} AAssetManager;

AResult a_asset_manager_create(AAssetManager **out);
void a_asset_manager_destroy(AAssetManager *asset_manager);

AResult a_asset_manager_init(AAssetManager *out);
void a_asset_manager_deinit(AAssetManager *asset_manager);

AResult a_asset_manager_get_texture(const AAssetManager *asset_manager, ATextureHandle handle,
                                    ATexture *out);

// Sprite
typedef struct ASprite ASprite;
typedef struct ASpriteArray ASpriteArray;

struct ASprite {
  AVector pos;
  uint texture_id;
  bool is_dynamic;
};

struct ASpriteArray {
  ASprite *items;
  uint len;
  uint capacity;
};

// Level
typedef struct ALevel ALevel;
typedef struct {
  uint (*width)(ALevel *level);
  uint (*height)(ALevel *level);
  uint (*wall)(ALevel *level, int idx_x, int idx_y);
  uint (*floor)(ALevel *level, int idx_x, int idx_y);
  uint (*ceil)(ALevel *level, int idx_x, int idx_y);
  uint (*unit_size)(ALevel *level);
  void (*sprites)(ALevel *level, ASprite **out, uint *count);
  void (*update)(ALevel *level, float delta_t);
  void (*destroy)(ALevel *level);
  StringArray *(*texture_paths)(ALevel *level);
} ALevelVtbl;

struct ALevel {
  const ALevelVtbl *vtbl;
  void *impl;
};

StringArray *a_level_get_texture_paths(ALevel *level);

// Raycaster Level
#define LEVEL_RAYCASTER_DEFAULT_MAP_UNIT_SIZE 20
#define LEVEL_RAYCASTER_DEFAULT_NUM_ASSETS 10

typedef enum {
  MAP_SECTION_NONE,
  MAP_SECTION_MAP,
  MAP_SECTION_CEIL,
  MAP_SECTION_FLOOR,
  MAP_SECTION_TEXTURES,
  MAP_SECTION_SPRITE_TYPES,
  MAP_SECTION_SPRITES
} LevelRaycasterFileSection;

typedef struct {
  char *path;
  float frame_delay;
  uint frame_count;
} ALevelRaycasterSpriteTypeDef;

typedef struct {
  ALevelRaycasterSpriteTypeDef *items;
  uint len;
  uint capacity;
} ALevelRaycasterSpriteTypes;

typedef struct {
  uint8_t **walls;
  uint8_t **ceil;
  uint8_t **floor;
  size_t unit_size;
  size_t width;
  size_t height;

  StringArray tex_paths;
  ALevelRaycasterSpriteTypes sprite_types;
  ASpriteArray sprites;
} LevelRaycasterMapData;

extern ALevelVtbl a_level_raycaster_vtbl;

// Camera
struct ACamera {
  AVector pos;
  AVector dir;
  AVector plane;
  float fov;
  uint width;
  uint height;
};

AResult a_camera_create(uint width, uint height, ACamera **out);
AResult a_camera_init(uint width, uint height, ACamera *out);
void a_camera_deinit(ACamera *cam);
void a_camera_destroy(ACamera *cam);

void a_camera_get_view_dimension(ACamera *cam, ADimensions *dims);
void a_camera_set_position(ACamera *cam, double pos_x, double pos_y);
void a_camera_set_direction(ACamera *cam, double dir_x, double dir_y);
void a_camera_set_fov(ACamera *cam, double fov_degrees);

void a_camera_rotate(ACamera *cam, double angle);

// Scene
typedef struct AScene AScene;

struct AScene {
  ACamera camera;
  ALevel *level;
  ATextureArray *textures;
};

// Actor
typedef struct AActor AActor;

typedef struct AActorVtbl {
  void (*update)(AActor *actor, ALevel *level, AInput *input, float delta_t);
  AVector (*get_position)(AActor *actor);
  AVector (*get_direction)(AActor *actor);
  uint32_t (*get_texture)(AActor *actor);
  void (*destroy)(AActor *actor);
} AActorVtbl;

struct AActor {
  const AActorVtbl *vtbl;
  void *impl;
  AVector pos;
  AVector dir;
  AVector velocity;
  int health;
  uint32_t texture_id;
};

void ae_actor_update(AActor *actor, ALevel *world, AInput *input, float delta_t);
AVector ae_actor_get_position(AActor *actor);
AVector ae_actor_get_direction(AActor *actor);
uint32_t ae_actor_get_texture(AActor *actor);
void ae_actor_destory(AActor *actor);

// Renderer
typedef struct ARenderer ARenderer;
typedef struct ARendererBackend ARendererBackend;
typedef struct ARendererBackendVtbl ARendererBackendVtbl;

typedef struct ARendererTechnique ARendererTechnique;
typedef struct ARendererTechniqueVtbl ARendererTechniqueVtbl;

typedef struct ARendererTechniqueRaycasterData ARendererTechniqueRaycasterData;

struct ARendererBackend {
  const ARendererBackendVtbl *vtbl;
  void *impl;
};

struct ARendererTechnique {
  const ARendererTechniqueVtbl *vtbl;
  void *impl;
};

struct ARenderer {
  ARendererBackend backend;
  ARendererTechnique technique;
};

struct ARendererBackendVtbl {
  void (*render)(const ARendererBackend *backend);
  uint32 *(*framebuffer)(const ARendererBackend *backend);
  double *(*zbuffer)(const ARendererBackend *backend);
  ADimensions *(*window_dims)(const ARendererBackend *backend);
  void (*destroy)(ARendererBackend *backend);
};

struct ARendererTechniqueVtbl {
  void (*process)(ARendererTechnique *technique);
  void (*destroy)(ARendererTechnique *technique);
};

struct ARendererTechniqueRaycasterData {
  AScene *scene;
  ADimensions *window_dimensions;
  uint32 *framebuffer;
  double *zbuffer;
};

AResult a_renderer_create(ARendererConfig config, ARenderer **out);
void a_renderer_destroy(ARenderer *renderer);

void a_renderer_backend_render(const ARendererBackend *backend);
uint32 *a_renderer_backend_framebuffer(const ARendererBackend *backend);
double *a_renderer_backend_zbuffer(const ARendererBackend *backend);
ADimensions *a_renderer_backend_window_dimensions(const ARendererBackend *backend);

void a_renderer_technique_process(ARendererTechnique *technique);

void a_renderer_technique_raycaster(AScene *scene, ARenderer *renderer);
AResult a_renderer_technique_raycaster_create(const ARendererTechniqueRaycasterData data_in,
                                              ARendererTechnique **out);
void a_renderer_technique_raycaster_destroy(ARendererTechnique *technique);
AResult a_renderer_technique_raycaster_init(const ARendererTechniqueRaycasterData data_in,
                                            ARendererTechnique *out);
void a_renderer_technique_raycaster_deinit(ARendererTechnique *technique);

// Input
typedef struct AInputVtbl AInputVtbl;
typedef struct AInput AInput;

struct AInput {
  const AInputVtbl *vtbl;
  void *impl;
};

struct AInputVtbl {
  bool (*down)(AInput *input, AKey key);
  bool (*pressed)(AInput *input, AKey key);
  void (*update)(AInput *input);
  void (*destroy)(AInput *input);
};

// Engine
typedef struct AEngine AEngine;

struct AEngine {
  AAssetManager asset_manager;
  ARenderer *renderer;
  ALevel *level;
  ACamera *camera;
  AInput *input;
  void *game_state;
  AEngineConfig config;

  float delta_time;
  float fps;
  float accumulator;

  uint last_time;
  uint current_time;
  uint frame_count;
  uint fps_timer;

  bool running;
};

#endif // I_ALPHA_H
