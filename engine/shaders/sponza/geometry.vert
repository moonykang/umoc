#include "../common.hlsl"

struct VSInput
{
    [[vk::location(0)]] float3 pos : POSITION;
    [[vk::location(1)]] float3 normal : NORMAL;
    [[vk::location(2)]] float2 uv : TEXCOORD;
};

struct VSOutput
{
	float4 pos : SV_POSITION;
    [[vk::location(0)]] float3 viewPos : POSITION0;
    [[vk::location(1)]] float3 normal : NORMAL0;
    [[vk::location(2)]] float2 uv : TEXCOORD0;
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
	VSOutput output = (VSOutput)0;

	output.pos = mul(sceneUBO.projection, mul(sceneUBO.view, mul(modelUBO.transform, float4(input.pos, 1.0f))));

	output.uv = input.uv;

	// Vertex position in view space
	output.viewPos = mul(sceneUBO.view, mul(modelUBO.transform, float4(input.pos, 1.0f))).xyz;

	// Normal in view space
	float3x3 normalMatrix = (float3x3)mul(sceneUBO.view, modelUBO.transform);
	output.normal = mul(normalMatrix, input.normal);

	return output;
}