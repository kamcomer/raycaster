#ifndef RAYCASTER_GPU_RENDERER_H
#define RAYCASTER_GPU_RENDERER_H

#include <SDL3/SDL_gpu.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RcGPURenderer RcGPURenderer;

struct RcGPURenderer
{
    SDL_GPUDevice *device;
    SDL_GPUCommandBuffer *cmd_buffer;
    SDL_Window *window;
    bool swapchain_acquired;

    SDL_GPUTexture *wall_textures[11];
    SDL_GPUTexture *floor_texture;
    SDL_GPUTexture *ceiling_texture;
    SDL_GPUTexture *sprite_textures[3];

    SDL_GPUBuffer *wall_vertex_buffer;
    size_t wall_vertex_buffer_size;

    SDL_GPUShader *wall_shader;
    SDL_GPUShader *sprite_shader;
    SDL_GPUShader *floor_shader;

    SDL_GPUGraphicsPipeline *wall_pipeline;
    SDL_GPUGraphicsPipeline *sprite_pipeline;
    SDL_GPUGraphicsPipeline *floor_pipeline;

    SDL_GPUSampler *sampler;
};

RcGPURenderer *rc_gpu_renderer_create(SDL_Window *window);
void rc_gpu_renderer_destroy(RcGPURenderer *r);

int rc_gpu_renderer_upload_textures(RcGPURenderer *r, uint32_t *textures[], int count);
int rc_gpu_renderer_set_floor_texture(RcGPURenderer *r, uint32_t *pixels);
int rc_gpu_renderer_set_ceiling_texture(RcGPURenderer *r, uint32_t *pixels);
int rc_gpu_renderer_set_sprite_texture(RcGPURenderer *r, int index, uint32_t *pixels);

void rc_gpu_renderer_begin_frame(RcGPURenderer *r);
void rc_gpu_renderer_end_frame(RcGPURenderer *r);

void rc_gpu_renderer_draw_walls(RcGPURenderer *r, float cam_x, float cam_y, 
                                 float dir_x, float dir_y, float plane_x, float plane_y,
                                 int screen_width, int screen_height,
                                 int *map, int map_width, int map_height,
                                 double *z_buffer, void *wall_data, int wall_count);

void rc_gpu_renderer_draw_sprites(RcGPURenderer *r, float cam_x, float cam_y,
                                   float dir_x, float dir_y, float plane_x, float plane_y,
                                   int screen_width, int screen_height,
                                   double *z_buffer, void *sprite_data, int sprite_count);

void rc_gpu_renderer_draw_floor_ceiling(RcGPURenderer *r, float cam_x, float cam_y,
                                          float dir_x, float dir_y, float plane_x, float plane_y,
                                          int screen_width, int screen_height);

#ifdef __cplusplus
}
#endif

#endif
