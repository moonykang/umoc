#include "../common.hlsl"

struct VSInput
{
    [[vk::location(0)]] float3 pos : POSITION;
    [[vk::location(1)]] float3 normal : NORMAL;
    [[vk::location(2)]] float2 uv : TEXCOORD;
    [[vk::location(3)]] float3 tangent : TANGENT;
    [[vk::location(4)]] float3 bitangent : BINORMAL;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    [[vk::location(0)]] float3 worldPos : POSITION0;
    [[vk::location(1)]] float2 uv : TEXCOORD;
    [[vk::location(2)]] float3 tangentLightPos : POSITION1;
    [[vk::location(3)]] float3 tangentViewPos : POSITION2;
    [[vk::location(4)]] float3 tangentFragPos : POSITION3;
};

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneUBO;
}

[[vk::binding(1, 0)]] cbuffer ubo
{
    SceneLight lightUBO;
}

[[vk::binding(0, 1)]] cbuffer ubo
{
    Model modelUBO;
}

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    float4 worldPos = mul(modelUBO.transform, float4(input.pos, 1.0f));
    output.worldPos = worldPos.xyz;
    output.uv = input.uv;

    float3 N = normalize(mul(modelUBO.transform, float4(input.normal, 0.0f)).xyz);
    float3 T = normalize(mul(modelUBO.transform, float4(input.tangent, 0.0f)).xyz);
    T = normalize(T - mul(dot(T, N), N));
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);


    output.tangentLightPos = mul(TBN, lightUBO.lights[0].pos.xyz);
    output.tangentViewPos = mul(TBN, sceneUBO.pos.xyz);
    output.tangentFragPos = mul(TBN, worldPos.xyz);

    output.pos = mul(sceneUBO.view_proj, worldPos);

    return output;
}