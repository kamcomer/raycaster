#include "raycaster/gpu_renderer.h"
#include "raycaster/types.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RcGPURenderer *rc_gpu_renderer_create(SDL_Window *window)
{
    SDL_GPUShaderFormat format_flags = SDL_GPU_SHADERFORMAT_MSL;
    
    RcGPURenderer *r = calloc(1, sizeof(RcGPURenderer));
    if (!r)
        return NULL;

    r->device = SDL_CreateGPUDevice(format_flags, true, NULL);
    if (!r->device)
    {
        fprintf(stderr, "Failed to create GPU device: %s\n", SDL_GetError());
        free(r);
        return NULL;
    }

    if (!SDL_ClaimWindowForGPUDevice(r->device, window))
    {
        fprintf(stderr, "Failed to claim window for GPU device: %s\n", SDL_GetError());
        SDL_DestroyGPUDevice(r->device);
        free(r);
        return NULL;
    }

    r->window = window;
    r->swapchain_acquired = false;

    r->cmd_buffer = SDL_AcquireGPUCommandBuffer(r->device);
    if (!r->cmd_buffer)
    {
        fprintf(stderr, "Failed to acquire command buffer: %s\n", SDL_GetError());
        SDL_ReleaseWindowFromGPUDevice(r->device, window);
        SDL_DestroyGPUDevice(r->device);
        free(r);
        return NULL;
    }

    printf("GPU device created successfully!\n");

    return r;
}

void rc_gpu_renderer_destroy(RcGPURenderer *r)
{
    if (!r)
        return;

    if (r->cmd_buffer)
    {
        SDL_SubmitGPUCommandBuffer(r->cmd_buffer);
    }

    if (r->device && r->window)
    {
        SDL_ReleaseWindowFromGPUDevice(r->device, r->window);
    }

    if (r->device)
    {
        SDL_DestroyGPUDevice(r->device);
    }

    free(r);
}

int rc_gpu_renderer_upload_textures(RcGPURenderer *r, uint32_t *textures[], int count)
{
    (void)r;
    (void)textures;
    (void)count;
    return 0;
}

int rc_gpu_renderer_set_floor_texture(RcGPURenderer *r, uint32_t *pixels)
{
    (void)r;
    (void)pixels;
    return 0;
}

int rc_gpu_renderer_set_ceiling_texture(RcGPURenderer *r, uint32_t *pixels)
{
    (void)r;
    (void)pixels;
    return 0;
}

int rc_gpu_renderer_set_sprite_texture(RcGPURenderer *r, int index, uint32_t *pixels)
{
    (void)r;
    (void)index;
    (void)pixels;
    return 0;
}

void rc_gpu_renderer_begin_frame(RcGPURenderer *r)
{
    if (!r || !r->device || !r->window)
        return;

    if (!r->cmd_buffer)
    {
        r->cmd_buffer = SDL_AcquireGPUCommandBuffer(r->device);
    }
    
    SDL_GPUTexture *swapchain = NULL;
    Uint32 w, h;
    if (SDL_WaitAndAcquireGPUSwapchainTexture(r->cmd_buffer, r->window, &swapchain, &w, &h))
    {
        r->swapchain_acquired = true;
        (void)w;
        (void)h;
    }
}

void rc_gpu_renderer_end_frame(RcGPURenderer *r)
{
    if (!r)
        return;

    if (r->swapchain_acquired)
    {
        r->swapchain_acquired = false;
    }

    if (r->cmd_buffer)
    {
        SDL_SubmitGPUCommandBuffer(r->cmd_buffer);
        r->cmd_buffer = NULL;
    }
}

void rc_gpu_renderer_draw_walls(RcGPURenderer *r, float cam_x, float cam_y, 
                                 float dir_x, float dir_y, float plane_x, float plane_y,
                                 int screen_width, int screen_height,
                                 int *map, int map_width, int map_height,
                                 double *z_buffer, void *wall_data, int wall_count)
{
    (void)r;
    (void)cam_x;
    (void)cam_y;
    (void)dir_x;
    (void)dir_y;
    (void)plane_x;
    (void)plane_y;
    (void)screen_width;
    (void)screen_height;
    (void)map;
    (void)map_width;
    (void)map_height;
    (void)z_buffer;
    (void)wall_data;
    (void)wall_count;
}

void rc_gpu_renderer_draw_sprites(RcGPURenderer *r, float cam_x, float cam_y,
                                   float dir_x, float dir_y, float plane_x, float plane_y,
                                   int screen_width, int screen_height,
                                   double *z_buffer, void *sprite_data, int sprite_count)
{
    (void)r;
    (void)cam_x;
    (void)cam_y;
    (void)dir_x;
    (void)dir_y;
    (void)plane_x;
    (void)plane_y;
    (void)screen_width;
    (void)screen_height;
    (void)z_buffer;
    (void)sprite_data;
    (void)sprite_count;
}

void rc_gpu_renderer_draw_floor_ceiling(RcGPURenderer *r, float cam_x, float cam_y,
                                          float dir_x, float dir_y, float plane_x, float plane_y,
                                          int screen_width, int screen_height)
{
    (void)r;
    (void)cam_x;
    (void)cam_y;
    (void)dir_x;
    (void)dir_y;
    (void)plane_x;
    (void)plane_y;
    (void)screen_width;
    (void)screen_height;
}
