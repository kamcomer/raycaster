#ifndef RAYCASTER_GPU_RENDERER_H
#define RAYCASTER_GPU_RENDERER_H

#include <SDL3/SDL_gpu.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_WALL_TEXTURES 11
#define MAX_SPRITE_TEXTURES 3
#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64
#define MAX_WALL_STRIPS 2048

typedef struct RcGPURenderer RcGPURenderer;

typedef struct {
  float x, y;
  int texture;
} SpriteData;

struct RcGPURenderer {
  SDL_GPUDevice *device;
  SDL_GPUCommandBuffer *cmd_buffer;
  SDL_GPUTexture *swapchain_texture;
  SDL_Window *window;
  bool swapchain_acquired;
  uint32_t swapchain_width;
  uint32_t swapchain_height;
  uint32_t depth_width;
  uint32_t depth_height;

  SDL_GPUTexture *wall_texture_array;
  int wall_texture_count;
  SDL_GPUTexture *floor_texture;
  SDL_GPUTexture *ceiling_texture;
  SDL_GPUTexture *sprite_textures[MAX_SPRITE_TEXTURES];
  int sprite_texture_count;
  SDL_GPUTexture *depth_texture;

  SDL_GPUBuffer *wall_vertex_buffer;
  SDL_GPUBuffer *wall_index_buffer;
  SDL_GPUBuffer *sprite_vertex_buffer;
  SDL_GPUBuffer *floor_vertex_buffer;
  SDL_GPUBuffer *wall_uniform_buffer;
  SDL_GPUBuffer *floor_uniform_buffer;
  size_t wall_vertex_buffer_size;
  size_t wall_index_buffer_size;
  size_t sprite_vertex_buffer_size;
  size_t floor_vertex_buffer_size;

  SDL_GPUShader *wall_vertex_shader;
  SDL_GPUShader *wall_fragment_shader;
  SDL_GPUShader *sprite_vertex_shader;
  SDL_GPUShader *sprite_fragment_shader;
  SDL_GPUShader *floor_vertex_shader;
  SDL_GPUShader *floor_fragment_shader;

  SDL_GPUGraphicsPipeline *wall_pipeline;
  SDL_GPUGraphicsPipeline *sprite_pipeline;
  SDL_GPUGraphicsPipeline *floor_pipeline;

  SDL_GPUSampler *sampler;
  SDL_GPUSampler *sampler_nearest;

  bool initialized;
  bool frame_begun;
};

RcGPURenderer *rc_gpu_renderer_create(SDL_Window *window);
void rc_gpu_renderer_destroy(RcGPURenderer *r);

int rc_gpu_renderer_upload_textures(RcGPURenderer *r, uint32_t *textures[], int count);
int rc_gpu_renderer_set_floor_texture(RcGPURenderer *r, uint32_t *pixels);
int rc_gpu_renderer_set_ceiling_texture(RcGPURenderer *r, uint32_t *pixels);
int rc_gpu_renderer_set_sprite_texture(RcGPURenderer *r, int index, uint32_t *pixels);

void rc_gpu_renderer_begin_frame(RcGPURenderer *r);
void rc_gpu_renderer_end_frame(RcGPURenderer *r);

void rc_gpu_renderer_draw_scene(RcGPURenderer *r, float cam_x, float cam_y, float dir_x,
                                float dir_y, float plane_x, float plane_y, int screen_width,
                                int screen_height, int *map, int map_width, int map_height,
                                void *intersects, int num_intersects, void *sprites,
                                int num_sprites);

#ifdef __cplusplus
}
#endif

#endif
