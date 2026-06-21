// Sprite shader for raycaster - billboard sprites with alpha
// SDL3 GPU requires: textures on set 0, uniform buffers on set 1

cbuffer Uniforms : register(b0, space1)
{
    float4x4 projection;
    float4x4 view;
    float4 camera_pos;
    float2 tex_size;
    float2 padding;
};

struct VertexInput
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float tex_id : TEXCOORD1;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float tex_id : TEXCOORD1;
    float distance : TEXCOORD2;
};

VertexOutput vertex_main(VertexInput input)
{
    VertexOutput output;
    output.position = mul(projection, mul(view, float4(input.position, 1.0)));
    output.texcoord = input.texcoord;
    output.tex_id = input.tex_id;
    output.distance = length(input.position - camera_pos.xyz);
    return output;
}

Texture2DArray sprite_textures : register(t0, space0);
SamplerState texture_sampler : register(s0, space0);

struct FragmentInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float tex_id : TEXCOORD1;
    float distance : TEXCOORD2;
};

float4 fragment_main(FragmentInput input) : SV_TARGET
{
    float4 color = sprite_textures.Sample(texture_sampler, float3(input.texcoord, input.tex_id));
    
    if (color.a < 0.1)
        discard;
    
    float fog_start = 5.0;
    float fog_end = 20.0;
    float fog_factor = clamp((input.distance - fog_start) / (fog_end - fog_start), 0.0, 1.0);
    float3 fog_color = float3(0.1, 0.1, 0.15);
    
    color.rgb = lerp(color.rgb, fog_color, fog_factor);
    
    return color;
}
