struct VSOutput
{
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

[[vk::binding(0, 2)]] TextureCube environMapTexture;
[[vk::binding(0, 2)]] SamplerState environMapSampler;

float4 main(VSOutput input) : SV_TARGET
{
    float3 I = normalize(input.worldPos - sceneUBO.cam_pos.xyz);
    float3 R = reflect(I, normalize(input.normal));

    float4 color = float4(environMapTexture.Sample(environMapSampler, R).xyz, 1.0f);

    return color;
}