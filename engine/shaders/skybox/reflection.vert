#include "../common.hlsl"

struct VSInput
{
    [[vk::location(0)]] float3 pos : POSITION;
    [[vk::location(1)]] float3 normal : NORMAL;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    [[vk::location(0)]] float3 worldPos : POSITION;
    [[vk::location(1)]] float3 normal : normal;
};

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneUBO;
}

[[vk::binding(0, 1)]] cbuffer ubo
{
    Model modelUBO;
}

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    output.normal = mul(modelUBO.normalMatrix, input.normal);
    float3 worldPos = mul(modelUBO.transform, float4(input.pos, 1.0f)).xyz;

    output.pos = mul(sceneUBO.view_proj, float4(worldPos, 1.0f));
    output.worldPos = worldPos;

    return output;
}