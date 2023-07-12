#include "../common.hlsl"

struct VSOutput
{
    [[vk::location(0)]] float3 worldPos : POSITION;
    [[vk::location(1)]] float3 normal : normal;
};

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneUBO;
}

[[vk::binding(0, 2)]] TextureCube environMapTexture;
[[vk::binding(0, 2)]] SamplerState environMapSampler;

float4 main(VSOutput input) : SV_TARGET
{
    float3 I = normalize(input.worldPos - sceneUBO.pos.xyz);
    float3 R = reflect(I, normalize(input.normal));

    float4 color = float4(environMapTexture.Sample(environMapSampler, R).xyz, 1.0f);

    return color;
}