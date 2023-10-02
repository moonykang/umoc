#include "../common.hlsl"

struct VSInput
{
    uint id : SV_VertexID;
};

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneView;
}

struct VSOutput
{
    float4 pos : SV_POSITION;
    [[vk::location(0)]] float3 nearPoint : POSITION0;
    [[vk::location(1)]] float3 farPoint : POSITION1;
};

// Grid position are in xy clipped space
static float3 gridPlane[4] = {
    float3(-1, -1, 0), float3(1, -1, 0),
    float3(-1, 1, 0), float3(1, 1, 0)
};

/*
struct SceneView
{
    float4x4 view_inverse;
    float4x4 proj_inverse;
    float4x4 view_proj_inverse;
    float4x4 prev_view_proj;
    float4x4 view_proj;
    float4x4 view;
    float4x4 projection;
    float4 pos;
	float nearPlane;
	float farPlane;
};
*/
float3 unprojectPoint(float x, float y, float z) {
    float4 unprojectedPoint =  mul(sceneView.view_inverse, mul(sceneView.proj_inverse, float4(x, y, z, 1.0f)));
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    float3 pos = gridPlane[input.id];
    output.nearPoint = unprojectPoint(pos.x, pos.y, 0.0f);
    output.farPoint = unprojectPoint(pos.x, pos.y, 1.0f);

    output.pos = float4(pos, 1.0f);

    return output;
}