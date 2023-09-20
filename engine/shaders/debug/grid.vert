#include "../common.hlsl"

struct VSInput
{
    uint id : SV_VertexID;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    [[vk::location(0)]] float3 nearPoint : POSITION0;
    [[vk::location(1)]] float3 farPoint : POSITION1;
};

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneView;
}

static float3 gridPlane[4] = {
    float3(-1.f, -1.f, 0), float3(1.f, -1.f, 0), float3(-1.f, 1.f, 0), float3(1.f, 1.f, 0)
};

float3 unprojectPoint(float x, float y, float z) {
    float4 unprojectedPoint =  mul(sceneView.view_proj_inverse, float4(x, y, z, 1.0));
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    float3 p = gridPlane[input.id];
    output.nearPoint = unprojectPoint(p.x, p.y, 0.f);
    output.farPoint = unprojectPoint(p.x, p.y, 1.f);
    output.pos = float4(p, 1.f);
    return output;
}