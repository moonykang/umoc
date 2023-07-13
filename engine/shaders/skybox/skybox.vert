#include "../common.hlsl"

struct VSInput
{
    [[vk::location(0)]] float3 pos : POSITION0;
    [[vk::location(1)]] float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    [[vk::location(0)]] float3 uvw : TEXCOORD0;
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
    output.uvw = input.pos;

    output.pos = mul(sceneUBO.view_proj, mul(modelUBO.transform, float4(input.pos, 1.0f)));
    return output;
}