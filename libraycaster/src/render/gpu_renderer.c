#include "raycaster/gpu_renderer.h"
#include "raycaster/types.h"
#include "actors/player.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3_shadercross/SDL_shadercross.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SHADER_DIR "shaders/"

typedef struct {
    float x, y, z;
    float u, v;
    float tex_id;
} WallVertex;

typedef struct {
    float x, y, z;
    float u, v;
    float tex_id;
} SpriteVertex;

typedef struct {
    float x, y;
    float u, v;
} FloorVertex;

static char *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f)
    {
        fprintf(stderr, "Failed to open file: %s\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(size + 1);
    if (!buffer)
    {
        fclose(f);
        return NULL;
    }

    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);

    if (out_size)
        *out_size = size;
    return buffer;
}

static SDL_GPUShader *compile_shader_from_hlsl(RcGPURenderer *r,
                                                const char *filename,
                                                SDL_ShaderCross_ShaderStage stage,
                                                const char *entrypoint)
{
    char path[256];
    snprintf(path, sizeof(path), "%s%s", SHADER_DIR, filename);

    size_t source_size;
    char *source = read_file(path, &source_size);
    if (!source)
        return NULL;

    SDL_ShaderCross_HLSL_Info hlsl_info = {
        .source = source,
        .entrypoint = entrypoint,
        .include_dir = SHADER_DIR,
        .defines = NULL,
        .shader_stage = stage,
        .props = 0
    };

    size_t spirv_size;
    void *spirv = SDL_ShaderCross_CompileSPIRVFromHLSL(&hlsl_info, &spirv_size);
    free(source);

    if (!spirv)
    {
        fprintf(stderr, "Failed to compile HLSL to SPIR-V: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_ShaderCross_SPIRV_Info spirv_info = {
        .bytecode = spirv,
        .bytecode_size = spirv_size,
        .entrypoint = entrypoint,
        .shader_stage = stage,
        .props = 0
    };

    SDL_ShaderCross_GraphicsShaderMetadata *metadata =
        SDL_ShaderCross_ReflectGraphicsSPIRV(spirv, spirv_size, 0);
    if (!metadata)
    {
        fprintf(stderr, "Failed to reflect SPIR-V shader\n");
        SDL_free(spirv);
        return NULL;
    }

    SDL_GPUShader *shader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(
        r->device, &spirv_info, &metadata->resource_info, 0);

    SDL_free(metadata);
    SDL_free(spirv);

    if (!shader)
    {
        fprintf(stderr, "Failed to create GPU shader from SPIR-V: %s\n", SDL_GetError());
        return NULL;
    }

    printf("Compiled shader: %s (stage: %d)\n", filename, stage);
    return shader;
}

static SDL_GPUSampler *create_sampler(RcGPURenderer *r, bool nearest)
{
    SDL_GPUSamplerCreateInfo createinfo = {
        .min_filter = nearest ? SDL_GPU_FILTER_NEAREST : SDL_GPU_FILTER_LINEAR,
        .mag_filter = nearest ? SDL_GPU_FILTER_NEAREST : SDL_GPU_FILTER_LINEAR,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
    };
    return SDL_CreateGPUSampler(r->device, &createinfo);
}

static bool create_pipelines(RcGPURenderer *r)
{
    r->wall_vertex_shader = compile_shader_from_hlsl(r, "wall.hlsl",
                                                      SDL_SHADERCROSS_SHADERSTAGE_VERTEX,
                                                      "vertex_main");
    if (!r->wall_vertex_shader)
        return false;

    r->wall_fragment_shader = compile_shader_from_hlsl(r, "wall.hlsl",
                                                        SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT,
                                                        "fragment_main");
    if (!r->wall_fragment_shader)
        return false;

    SDL_GPUGraphicsPipelineCreateInfo create_info = {
        .vertex_shader = r->wall_vertex_shader,
        .fragment_shader = r->wall_fragment_shader,
        .vertex_input_state = {
            .num_vertex_attributes = 3,
            .vertex_attributes = (SDL_GPUVertexAttribute[]){
                {.location = 0, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, .offset = 0},
                {.location = 1, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, .offset = 12},
                {.location = 2, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT, .offset = 20},
            },
            .num_vertex_buffers = 1,
            .vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){
                {.slot = 0, .pitch = sizeof(WallVertex), .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX}
            }
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .rasterizer_state = {
            .fill_mode = SDL_GPU_FILLMODE_FILL,
            .cull_mode = SDL_GPU_CULLMODE_NONE,
            .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE
        },
        .depth_stencil_state = {
            .compare_op = SDL_GPU_COMPAREOP_LESS,
            .enable_depth_test = true,
            .enable_depth_write = true
        },
        .target_info = {
            .num_color_targets = 1,
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]){
                {.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM}
            },
            .has_depth_stencil_target = true,
            .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT
        }
    };

    r->wall_pipeline = SDL_CreateGPUGraphicsPipeline(r->device, &create_info);
    if (!r->wall_pipeline)
    {
        fprintf(stderr, "Failed to create wall pipeline: %s\n", SDL_GetError());
        return false;
    }

    r->sprite_vertex_shader = compile_shader_from_hlsl(r, "sprite.hlsl",
                                                        SDL_SHADERCROSS_SHADERSTAGE_VERTEX,
                                                        "vertex_main");
    if (!r->sprite_vertex_shader)
        return false;

    r->sprite_fragment_shader = compile_shader_from_hlsl(r, "sprite.hlsl",
                                                         SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT,
                                                         "fragment_main");
    if (!r->sprite_fragment_shader)
        return false;

    create_info.vertex_shader = r->sprite_vertex_shader;
    create_info.fragment_shader = r->sprite_fragment_shader;
    create_info.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS;

    r->sprite_pipeline = SDL_CreateGPUGraphicsPipeline(r->device, &create_info);
    if (!r->sprite_pipeline)
    {
        fprintf(stderr, "Failed to create sprite pipeline: %s\n", SDL_GetError());
        return false;
    }

    r->floor_vertex_shader = compile_shader_from_hlsl(r, "floor.hlsl",
                                                     SDL_SHADERCROSS_SHADERSTAGE_VERTEX,
                                                     "vertex_main");
    if (!r->floor_vertex_shader)
        return false;

    r->floor_fragment_shader = compile_shader_from_hlsl(r, "floor.hlsl",
                                                         SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT,
                                                         "fragment_main");
    if (!r->floor_fragment_shader)
        return false;

    create_info.vertex_shader = r->floor_vertex_shader;
    create_info.fragment_shader = r->floor_fragment_shader;
    create_info.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_ALWAYS;
    create_info.depth_stencil_state.enable_depth_test = false;
    create_info.depth_stencil_state.enable_depth_write = false;

    r->floor_pipeline = SDL_CreateGPUGraphicsPipeline(r->device, &create_info);
    if (!r->floor_pipeline)
    {
        fprintf(stderr, "Failed to create floor pipeline: %s\n", SDL_GetError());
        return false;
    }

    printf("All pipelines created successfully!\n");
    return true;
}

static bool upload_texture_layer(RcGPURenderer *r, SDL_GPUTexture *texture, uint32_t *pixels, int width, int height, int layer)
{
    SDL_GPUTransferBufferCreateInfo transfer_info = {
        .size = width * height * 4,
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD
    };
    SDL_GPUTransferBuffer *transfer = SDL_CreateGPUTransferBuffer(r->device, &transfer_info);
    if (!transfer)
    {
        fprintf(stderr, "Failed to create transfer buffer: %s\n", SDL_GetError());
        return false;
    }

    uint32_t *data = SDL_MapGPUTransferBuffer(r->device, transfer, false);
    if (!data)
    {
        fprintf(stderr, "Failed to map transfer buffer: %s\n", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(r->device, transfer);
        return false;
    }

    memcpy(data, pixels, width * height * 4);
    SDL_UnmapGPUTransferBuffer(r->device, transfer);

    if (!r->cmd_buffer)
        r->cmd_buffer = SDL_AcquireGPUCommandBuffer(r->device);

    SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(r->cmd_buffer);
    
    SDL_GPUTextureTransferInfo source = {
        .transfer_buffer = transfer,
        .offset = 0,
        .pixels_per_row = (Uint32)width,
        .rows_per_layer = (Uint32)height
    };
    
    SDL_GPUTextureRegion dest = {
        .texture = texture,
        .w = (Uint32)width,
        .h = (Uint32)height,
        .d = 1,
        .layer = layer
    };
    
    SDL_UploadToGPUTexture(copy_pass, &source, &dest, false);
    
    SDL_EndGPUCopyPass(copy_pass);

    SDL_ReleaseGPUTransferBuffer(r->device, transfer);

    return true;
}

static SDL_GPUTexture *create_texture_2d_array(RcGPURenderer *r, int width, int height, int layers)
{
    SDL_GPUTextureCreateInfo tex_desc = {
        .type = SDL_GPU_TEXTURETYPE_2D_ARRAY,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .width = (Uint32)width,
        .height = (Uint32)height,
        .layer_count_or_depth = (Uint32)layers,
        .num_levels = 1,
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER
    };

    return SDL_CreateGPUTexture(r->device, &tex_desc);
}

static SDL_GPUTexture *create_depth_texture(RcGPURenderer *r, int width, int height)
{
    SDL_GPUTextureCreateInfo tex_desc = {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
        .width = (Uint32)width,
        .height = (Uint32)height,
        .layer_count_or_depth = 1,
        .num_levels = 1,
        .usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET
    };

    return SDL_CreateGPUTexture(r->device, &tex_desc);
}

RcGPURenderer *rc_gpu_renderer_create(SDL_Window *window)
{
    if (!SDL_ShaderCross_Init())
    {
        fprintf(stderr, "Failed to initialize SDL ShaderCross: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_GPUShaderFormat format_flags = SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_SPIRV;

    RcGPURenderer *r = calloc(1, sizeof(RcGPURenderer));
    if (!r)
        return NULL;

    r->device = SDL_CreateGPUDevice(format_flags, true, "metal");
    if (!r->device)
    {
        fprintf(stderr, "Failed to create GPU device: %s\n", SDL_GetError());
        free(r);
        SDL_ShaderCross_Quit();
        return NULL;
    }

    if (!SDL_ClaimWindowForGPUDevice(r->device, window))
    {
        fprintf(stderr, "Failed to claim window for GPU device: %s\n", SDL_GetError());
        SDL_DestroyGPUDevice(r->device);
        free(r);
        SDL_ShaderCross_Quit();
        return NULL;
    }

    r->window = window;
    r->swapchain_acquired = false;

    r->sampler = create_sampler(r, false);
    r->sampler_nearest = create_sampler(r, true);
    if (!r->sampler || !r->sampler_nearest)
    {
        fprintf(stderr, "Failed to create samplers\n");
        rc_gpu_renderer_destroy(r);
        return NULL;
    }

    if (!create_pipelines(r))
    {
        fprintf(stderr, "Failed to create pipelines\n");
        rc_gpu_renderer_destroy(r);
        return NULL;
    }

    r->initialized = true;
    r->frame_begun = false;
    printf("GPU renderer created successfully!\n");
    return r;
}

void rc_gpu_renderer_destroy(RcGPURenderer *r)
{
    if (!r)
        return;

    if (r->cmd_buffer)
    {
        SDL_SubmitGPUCommandBuffer(r->cmd_buffer);
        r->cmd_buffer = NULL;
    }

    if (r->device && r->window)
    {
        SDL_ReleaseWindowFromGPUDevice(r->device, r->window);
    }

    if (r->sampler)
        SDL_ReleaseGPUSampler(r->device, r->sampler);
    if (r->sampler_nearest)
        SDL_ReleaseGPUSampler(r->device, r->sampler_nearest);

    if (r->wall_pipeline)
        SDL_ReleaseGPUGraphicsPipeline(r->device, r->wall_pipeline);
    if (r->sprite_pipeline)
        SDL_ReleaseGPUGraphicsPipeline(r->device, r->sprite_pipeline);
    if (r->floor_pipeline)
        SDL_ReleaseGPUGraphicsPipeline(r->device, r->floor_pipeline);

    if (r->wall_vertex_shader)
        SDL_ReleaseGPUShader(r->device, r->wall_vertex_shader);
    if (r->wall_fragment_shader)
        SDL_ReleaseGPUShader(r->device, r->wall_fragment_shader);
    if (r->sprite_vertex_shader)
        SDL_ReleaseGPUShader(r->device, r->sprite_vertex_shader);
    if (r->sprite_fragment_shader)
        SDL_ReleaseGPUShader(r->device, r->sprite_fragment_shader);
    if (r->floor_vertex_shader)
        SDL_ReleaseGPUShader(r->device, r->floor_vertex_shader);
    if (r->floor_fragment_shader)
        SDL_ReleaseGPUShader(r->device, r->floor_fragment_shader);

    if (r->wall_texture_array)
        SDL_ReleaseGPUTexture(r->device, r->wall_texture_array);
    if (r->sprite_textures[0])
        SDL_ReleaseGPUTexture(r->device, r->sprite_textures[0]);
    if (r->floor_texture)
        SDL_ReleaseGPUTexture(r->device, r->floor_texture);
    if (r->ceiling_texture)
        SDL_ReleaseGPUTexture(r->device, r->ceiling_texture);
    if (r->depth_texture)
        SDL_ReleaseGPUTexture(r->device, r->depth_texture);

    if (r->wall_vertex_buffer)
        SDL_ReleaseGPUBuffer(r->device, r->wall_vertex_buffer);
    if (r->wall_index_buffer)
        SDL_ReleaseGPUBuffer(r->device, r->wall_index_buffer);
    if (r->sprite_vertex_buffer)
        SDL_ReleaseGPUBuffer(r->device, r->sprite_vertex_buffer);
    if (r->floor_vertex_buffer)
        SDL_ReleaseGPUBuffer(r->device, r->floor_vertex_buffer);

    if (r->device)
        SDL_DestroyGPUDevice(r->device);

    SDL_ShaderCross_Quit();
    free(r);
}

int rc_gpu_renderer_upload_textures(RcGPURenderer *r, uint32_t *textures[], int count)
{
    if (!r || !r->device || !textures)
        return -1;

    if (r->wall_texture_array)
        SDL_ReleaseGPUTexture(r->device, r->wall_texture_array);

    int upload_count = count < MAX_WALL_TEXTURES ? count : MAX_WALL_TEXTURES;
    r->wall_texture_count = upload_count;

    r->wall_texture_array = create_texture_2d_array(r, TEXTURE_WIDTH, TEXTURE_HEIGHT, MAX_WALL_TEXTURES);
    if (!r->wall_texture_array)
    {
        fprintf(stderr, "Failed to create wall texture array: %s\n", SDL_GetError());
        return -1;
    }

    for (int i = 0; i < upload_count; i++)
    {
        if (!upload_texture_layer(r, r->wall_texture_array, textures[i], TEXTURE_WIDTH, TEXTURE_HEIGHT, i))
        {
            fprintf(stderr, "Failed to upload wall texture %d\n", i);
        }
        else
        {
            printf("Uploaded wall texture %d to layer %d\n", i, i);
        }
    }

    if (r->cmd_buffer)
    {
        SDL_SubmitGPUCommandBuffer(r->cmd_buffer);
        r->cmd_buffer = NULL;
    }

    return 0;
}

int rc_gpu_renderer_set_floor_texture(RcGPURenderer *r, uint32_t *pixels)
{
    if (!r || !r->device || !pixels)
        return -1;

    if (r->floor_texture)
        SDL_ReleaseGPUTexture(r->device, r->floor_texture);

    SDL_GPUTextureCreateInfo tex_desc = {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .width = TEXTURE_WIDTH,
        .height = TEXTURE_HEIGHT,
        .layer_count_or_depth = 1,
        .num_levels = 1,
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER
    };

    r->floor_texture = SDL_CreateGPUTexture(r->device, &tex_desc);
    if (!r->floor_texture)
    {
        fprintf(stderr, "Failed to create floor texture: %s\n", SDL_GetError());
        return -1;
    }

    if (!upload_texture_layer(r, r->floor_texture, pixels, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0))
    {
        fprintf(stderr, "Failed to upload floor texture\n");
        return -1;
    }

    if (r->cmd_buffer)
    {
        SDL_SubmitGPUCommandBuffer(r->cmd_buffer);
        r->cmd_buffer = NULL;
    }

    return 0;
}

int rc_gpu_renderer_set_ceiling_texture(RcGPURenderer *r, uint32_t *pixels)
{
    if (!r || !r->device || !pixels)
        return -1;

    if (r->ceiling_texture)
        SDL_ReleaseGPUTexture(r->device, r->ceiling_texture);

    SDL_GPUTextureCreateInfo tex_desc = {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .width = TEXTURE_WIDTH,
        .height = TEXTURE_HEIGHT,
        .layer_count_or_depth = 1,
        .num_levels = 1,
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER
    };

    r->ceiling_texture = SDL_CreateGPUTexture(r->device, &tex_desc);
    if (!r->ceiling_texture)
    {
        fprintf(stderr, "Failed to create ceiling texture: %s\n", SDL_GetError());
        return -1;
    }

    if (!upload_texture_layer(r, r->ceiling_texture, pixels, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0))
    {
        fprintf(stderr, "Failed to upload ceiling texture\n");
        return -1;
    }

    if (r->cmd_buffer)
    {
        SDL_SubmitGPUCommandBuffer(r->cmd_buffer);
        r->cmd_buffer = NULL;
    }

    return 0;
}

int rc_gpu_renderer_set_sprite_texture(RcGPURenderer *r, int index, uint32_t *pixels)
{
    if (!r || !r->device || !pixels || index < 0 || index >= MAX_SPRITE_TEXTURES)
        return -1;

    if (r->sprite_textures[index])
        SDL_ReleaseGPUTexture(r->device, r->sprite_textures[index]);

    SDL_GPUTextureCreateInfo tex_desc = {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .width = TEXTURE_WIDTH,
        .height = TEXTURE_HEIGHT,
        .layer_count_or_depth = 1,
        .num_levels = 1,
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER
    };

    r->sprite_textures[index] = SDL_CreateGPUTexture(r->device, &tex_desc);
    if (!r->sprite_textures[index])
    {
        fprintf(stderr, "Failed to create sprite texture %d: %s\n", index, SDL_GetError());
        return -1;
    }

    if (!upload_texture_layer(r, r->sprite_textures[index], pixels, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0))
    {
        fprintf(stderr, "Failed to upload sprite texture %d\n", index);
        return -1;
    }

    if (index >= r->sprite_texture_count)
        r->sprite_texture_count = index + 1;

    if (r->cmd_buffer)
    {
        SDL_SubmitGPUCommandBuffer(r->cmd_buffer);
        r->cmd_buffer = NULL;
    }

    return 0;
}

static void ensure_buffers(RcGPURenderer *r, int screen_width, int screen_height)
{
    size_t wall_vertex_size = MAX_WALL_STRIPS * 4 * sizeof(WallVertex);
    size_t wall_index_size = MAX_WALL_STRIPS * 6 * sizeof(uint16_t);
    size_t floor_vertex_size = 6 * sizeof(FloorVertex);

    if (!r->wall_vertex_buffer || r->wall_vertex_buffer_size < wall_vertex_size)
    {
        if (r->wall_vertex_buffer)
            SDL_ReleaseGPUBuffer(r->device, r->wall_vertex_buffer);
        SDL_GPUBufferCreateInfo buffer_info = {
            .size = wall_vertex_size,
            .usage = SDL_GPU_BUFFERUSAGE_VERTEX
        };
        r->wall_vertex_buffer = SDL_CreateGPUBuffer(r->device, &buffer_info);
        r->wall_vertex_buffer_size = wall_vertex_size;
    }

    if (!r->wall_index_buffer || r->wall_index_buffer_size < wall_index_size)
    {
        if (r->wall_index_buffer)
            SDL_ReleaseGPUBuffer(r->device, r->wall_index_buffer);
        SDL_GPUBufferCreateInfo buffer_info = {
            .size = wall_index_size,
            .usage = SDL_GPU_BUFFERUSAGE_INDEX
        };
        r->wall_index_buffer = SDL_CreateGPUBuffer(r->device, &buffer_info);
        r->wall_index_buffer_size = wall_index_size;
    }

    if (!r->floor_vertex_buffer || r->floor_vertex_buffer_size < floor_vertex_size)
    {
        if (r->floor_vertex_buffer)
            SDL_ReleaseGPUBuffer(r->device, r->floor_vertex_buffer);
        SDL_GPUBufferCreateInfo buffer_info = {
            .size = floor_vertex_size,
            .usage = SDL_GPU_BUFFERUSAGE_VERTEX
        };
        r->floor_vertex_buffer = SDL_CreateGPUBuffer(r->device, &buffer_info);
        r->floor_vertex_buffer_size = floor_vertex_size;
    }

    if (!r->depth_texture || r->depth_width != screen_width || r->depth_height != screen_height)
    {
        if (r->depth_texture)
            SDL_ReleaseGPUTexture(r->device, r->depth_texture);
        r->depth_texture = create_depth_texture(r, screen_width, screen_height);
        r->depth_width = screen_width;
        r->depth_height = screen_height;
    }
}

static void upload_buffer_data(RcGPURenderer *r, SDL_GPUBuffer *buffer, const void *data, size_t size)
{
    SDL_GPUTransferBufferCreateInfo transfer_info = {
        .size = size,
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD
    };
    SDL_GPUTransferBuffer *transfer = SDL_CreateGPUTransferBuffer(r->device, &transfer_info);
    if (!transfer)
        return;

    void *mapped = SDL_MapGPUTransferBuffer(r->device, transfer, false);
    if (mapped)
    {
        memcpy(mapped, data, size);
        SDL_UnmapGPUTransferBuffer(r->device, transfer);

        if (!r->cmd_buffer)
            r->cmd_buffer = SDL_AcquireGPUCommandBuffer(r->device);

        SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(r->cmd_buffer);
        SDL_GPUTransferBufferLocation src = {.transfer_buffer = transfer, .offset = 0};
        SDL_GPUBufferRegion dst = {.buffer = buffer, .offset = 0, .size = (Uint32)size};
        SDL_UploadToGPUBuffer(copy_pass, &src, &dst, false);
        SDL_EndGPUCopyPass(copy_pass);
    }

    SDL_ReleaseGPUTransferBuffer(r->device, transfer);
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
    if (SDL_WaitAndAcquireGPUSwapchainTexture(r->cmd_buffer, r->window, &swapchain,
                                               &r->swapchain_width, &r->swapchain_height))
    {
        r->swapchain_acquired = true;
        r->swapchain_texture = swapchain;
    }
    
    ensure_buffers(r, r->swapchain_width, r->swapchain_height);
    
    r->frame_begun = true;
}

void rc_gpu_renderer_end_frame(RcGPURenderer *r)
{
    if (!r)
        return;

    if (r->swapchain_acquired)
    {
        r->swapchain_acquired = false;
        r->swapchain_texture = NULL;
    }

    if (r->cmd_buffer)
    {
        SDL_SubmitGPUCommandBuffer(r->cmd_buffer);
        r->cmd_buffer = NULL;
    }
    
    r->frame_begun = false;
}

void rc_gpu_renderer_draw_scene(RcGPURenderer *r,
                                float cam_x, float cam_y,
                                float dir_x, float dir_y,
                                float plane_x, float plane_y,
                                int screen_width, int screen_height,
                                int *map, int map_width, int map_height,
                                void *intersects_ptr, int num_intersects,
                                void *sprites_ptr, int num_sprites)
{
  (void)cam_x;
  (void)cam_y;
  (void)dir_x;
  (void)dir_y;
  (void)plane_x;
  (void)plane_y;
  (void)sprites_ptr;
  (void)num_sprites;
  (void)map;
  (void)map_width;
  (void)map_height;
  
  if (!r || !r->frame_begun || !r->swapchain_acquired)
    return;

  WallIntersect *intersects = (WallIntersect *)intersects_ptr;

    if (num_intersects > 0)
    {
        size_t vertex_data_size = num_intersects * 4 * sizeof(WallVertex);
        size_t index_data_size = num_intersects * 6 * sizeof(uint16_t);
        
        WallVertex *vertices = malloc(vertex_data_size);
        uint16_t *indices = malloc(index_data_size);
        if (!vertices || !indices)
        {
            free(vertices);
            free(indices);
            return;
        }
        
        int vert_idx = 0;
        int index_idx = 0;
        
        for (int x = 0; x < num_intersects && x < MAX_WALL_STRIPS; x++)
        {
            WallIntersect *inter = &intersects[x];
            
            if (inter->perp_wall_distance <= 0)
                continue;
            
            float line_height = (float)screen_height / (float)inter->perp_wall_distance;
            float draw_start = -line_height / 2.0f + screen_height / 2.0f;
            float draw_end = line_height / 2.0f + screen_height / 2.0f;
            
            if (draw_start < 0)
                draw_start = 0;
            if (draw_end >= screen_height)
                draw_end = screen_height - 1;
            
            float tex_x = inter->vect.x * TEXTURE_WIDTH;
            if (inter->side == 0 && inter->ray_dir.x > 0)
                tex_x = TEXTURE_WIDTH - tex_x - 1;
            if (inter->side == 1 && inter->ray_dir.y < 0)
                tex_x = TEXTURE_WIDTH - tex_x - 1;
            
            float u = tex_x / TEXTURE_WIDTH;
            
            int map_idx = inter->map_y * map_width + inter->map_x;
            int tex_id = (map_idx >= 0 && map_idx < map_width * map_height) ? map[map_idx] - 1 : 0;
            if (tex_id < 0)
                tex_id = 0;
            if (tex_id >= MAX_WALL_TEXTURES)
                tex_id = MAX_WALL_TEXTURES - 1;
            
            float x0 = (float)x / screen_width * 2.0f - 1.0f;
            float x1 = (float)(x + 1) / screen_width * 2.0f - 1.0f;
            float y0 = draw_start / screen_height * 2.0f - 1.0f;
            float y1 = draw_end / screen_height * 2.0f - 1.0f;
            
            vertices[vert_idx + 0] = (WallVertex){x0, y0, 0.0f, u, 0.0f, (float)tex_id};
            vertices[vert_idx + 1] = (WallVertex){x1, y0, 0.0f, u, 0.0f, (float)tex_id};
            vertices[vert_idx + 2] = (WallVertex){x0, y1, 0.0f, u, 1.0f, (float)tex_id};
            vertices[vert_idx + 3] = (WallVertex){x1, y1, 0.0f, u, 1.0f, (float)tex_id};
            
            uint16_t base = vert_idx;
            indices[index_idx++] = base + 0;
            indices[index_idx++] = base + 1;
            indices[index_idx++] = base + 2;
            indices[index_idx++] = base + 1;
            indices[index_idx++] = base + 3;
            indices[index_idx++] = base + 2;
            
            vert_idx += 4;
        }
        
        int num_vertices_to_upload = vert_idx;
        int num_indices_to_upload = index_idx;
        
        if (num_vertices_to_upload > 0)
        {
            upload_buffer_data(r, r->wall_vertex_buffer, vertices, num_vertices_to_upload * sizeof(WallVertex));
            upload_buffer_data(r, r->wall_index_buffer, indices, num_indices_to_upload * sizeof(uint16_t));
        }
        
        free(vertices);
        free(indices);
    }

    FloorVertex floor_vertices[] = {
        {-1.0f, -1.0f, 0.0f, 0.0f},
        { 1.0f, -1.0f, 1.0f, 0.0f},
        {-1.0f,  1.0f, 0.0f, 1.0f},
        { 1.0f, -1.0f, 1.0f, 0.0f},
        { 1.0f,  1.0f, 1.0f, 1.0f},
        {-1.0f,  1.0f, 0.0f, 1.0f},
    };
    upload_buffer_data(r, r->floor_vertex_buffer, floor_vertices, sizeof(floor_vertices));

    SDL_GPUColorTargetInfo color_target = {
        .texture = r->swapchain_texture,
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
        .clear_color = {0.1f, 0.1f, 0.15f, 1.0f}
    };

    SDL_GPUDepthStencilTargetInfo depth_target = {
        .texture = r->depth_texture,
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
        .clear_depth = 1.0f
    };

    SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(r->cmd_buffer, &color_target, 1, &depth_target);
    if (!render_pass)
    {
        fprintf(stderr, "Failed to begin render pass: %s\n", SDL_GetError());
        return;
    }

    if (num_intersects > 0 && r->wall_vertex_buffer && r->wall_index_buffer)
    {
        SDL_BindGPUGraphicsPipeline(render_pass, r->wall_pipeline);

        SDL_GPUBufferBinding vertex_binding = {.buffer = r->wall_vertex_buffer, .offset = 0};
        SDL_BindGPUVertexBuffers(render_pass, 0, &vertex_binding, 1);

        SDL_GPUBufferBinding index_binding = {.buffer = r->wall_index_buffer, .offset = 0};
        SDL_BindGPUIndexBuffer(render_pass, &index_binding, SDL_GPU_INDEXELEMENTSIZE_16BIT);

        if (r->wall_texture_array)
        {
            SDL_GPUTextureSamplerBinding texture_binding = {.texture = r->wall_texture_array, .sampler = r->sampler};
            SDL_BindGPUFragmentSamplers(render_pass, 0, &texture_binding, 1);
        }

        int num_indices = 0;
        for (int x = 0; x < num_intersects && x < MAX_WALL_STRIPS; x++)
        {
            if (intersects[x].perp_wall_distance > 0)
                num_indices += 6;
        }

        if (num_indices > 0)
        {
            SDL_DrawGPUIndexedPrimitives(render_pass, num_indices, 1, 0, 0, 0);
        }
    }

    SDL_BindGPUGraphicsPipeline(render_pass, r->floor_pipeline);

    SDL_GPUBufferBinding floor_vertex_binding = {.buffer = r->floor_vertex_buffer, .offset = 0};
    SDL_BindGPUVertexBuffers(render_pass, 0, &floor_vertex_binding, 1);

    if (r->floor_texture && r->ceiling_texture)
    {
        SDL_GPUTextureSamplerBinding texture_bindings[2] = {
            {.texture = r->floor_texture, .sampler = r->sampler},
            {.texture = r->ceiling_texture, .sampler = r->sampler}
        };
        SDL_BindGPUFragmentSamplers(render_pass, 0, texture_bindings, 2);
    }

    SDL_DrawGPUPrimitives(render_pass, 6, 1, 0, 0);

    SDL_EndGPURenderPass(render_pass);
}
