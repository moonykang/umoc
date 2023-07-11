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

struct SceneUBO
{
    float4x4 view_inverse;
    float4x4 proj_inverse;
    float4x4 view_proj_inverse;
    float4x4 prev_view_proj;
    float4x4 view_proj;
    float4 cam_pos;
};

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneUBO sceneUBO;
}

struct ModelUBO
{
    float4x4 transform;
    float3x3 transform_inverse;
};

[[vk::binding(0, 1)]] cbuffer ubo
{
    ModelUBO modelUBO;
}

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    output.normal = mul(modelUBO.transform_inverse, input.normal);
    float3 worldPos = mul(modelUBO.transform, float4(input.pos, 1.0f)).xyz;

    output.pos = mul(sceneUBO.view_proj, float4(worldPos, 1.0f));
    output.worldPos = worldPos;

    return output;
}