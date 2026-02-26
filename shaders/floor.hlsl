// Floor/Ceiling shader for raycaster
// SDL3 GPU requires: textures on set 0, uniform buffers on set 1

cbuffer Uniforms : register(b0, space1)
{
    float4x4 projection;
    float4x4 view;
    float4 camera_pos;
    float4 camera_dir;
    float4 camera_plane;
    float2 tex_size;
    float2 screen_res;
};

struct VertexInput
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD0;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 screen_pos : TEXCOORD0;
    float3 ray_dir : TEXCOORD1;
};

VertexOutput vertex_main(VertexInput input)
{
    VertexOutput output;
    output.position = float4(input.position, 1.0);
    output.screen_pos = input.texcoord;
    
    float2 uv = input.texcoord * 2.0 - 1.0;
    output.ray_dir = normalize(camera_dir.xyz + camera_plane.xyz * uv.x * (screen_res.x / screen_res.y) + float3(0, camera_plane.y, 0) * uv.y);
    
    return output;
}

Texture2D floor_texture : register(t0, space0);
Texture2D ceiling_texture : register(t1, space0);
SamplerState texture_sampler : register(s0, space0);

struct FragmentInput
{
    float4 position : SV_POSITION;
    float2 screen_pos : TEXCOORD0;
    float3 ray_dir : TEXCOORD1;
};

float4 fragment_main(FragmentInput input) : SV_TARGET
{
    float floor_dist = 1000.0 / -input.ray_dir.y;
    float3 floor_pos = camera_pos.xyz + input.ray_dir * floor_dist;
    float2 floor_texcoord = floor_pos.xz * 0.5 + 0.5;
    float4 floor_color = floor_texture.Sample(texture_sampler, floor_texcoord);
    
    float ceiling_dist = 1000.0 / input.ray_dir.y;
    float3 ceiling_pos = camera_pos.xyz + input.ray_dir * ceiling_dist;
    float2 ceiling_texcoord = ceiling_pos.xz * 0.5 + 0.5;
    float4 ceiling_color = ceiling_texture.Sample(texture_sampler, ceiling_texcoord);
    
    float y = (input.screen_pos.y + 1.0) * 0.5;
    float4 color = y < 0.5 ? ceiling_color : floor_color;
    
    return color;
}
