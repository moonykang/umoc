struct VSInput
{
    [[vk::location(0)]] float3 pos : POSITION0;
    [[vk::location(1)]] float3 color : COLOR0;
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

struct LightUBO
{
    float4 pos;
    float gamma;
    float exposure;
};

[[vk::binding(1, 0)]] cbuffer ubo
{
    LightUBO lightUBO;
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


struct VSOutput
{
    float4 pos : SV_POSITION;
    [[vk::location(0)]] float3 color : COLOR0;
};

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    output.color = input.color;
    output.pos = mul(sceneUBO.view_proj, mul(modelUBO.transform, float4(input.pos.xzy, 1.0)));

    return output;
}