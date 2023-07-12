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
    [[vk::location(0)]] float3 viewPos : POSITION0;
    [[vk::location(1)]] float3 normal : NORMAL0;
    [[vk::location(2)]] float2 uv : TEXCOORD0;
    [[vk::location(3)]] float3 tangent : TEXCOORD1;
};

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneUBO;
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
	VSOutput output = (VSOutput)0;

    float4 viewPos = mul(sceneUBO.view, mul(modelUBO.transform, float4(input.pos, 1.0f)));
    output.viewPos = viewPos.xyz;
    output.uv = input.uv;

    float3x3 normalMatrix = transpose(inverse((float3x3) mul(sceneUBO.view, modelUBO.transform)));
    output.normal = mul(normalMatrix, input.normal);
    output.pos = mul(sceneUBO.proj, viewPos);

	return output;
}