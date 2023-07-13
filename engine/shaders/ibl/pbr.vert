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
    [[vk::location(0)]] float3 worldPos : POSITION;
    [[vk::location(1)]] float3 normal : NORMAL;
    [[vk::location(2)]] float2 uv : TEXCOORD;
};

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneView;
}

[[vk::binding(0, 1)]] cbuffer ubo
{
    Model model;
}

VSOutput main(VSInput input)
{
	VSOutput output = (VSOutput)0;

	float3 worldPos = mul(model.transform, float4(input.pos, 1.0)).xyz;
	output.normal = mul((float3x3)model.transform, input.normal);
	output.pos = mul(sceneView.view_proj, float4(worldPos, 1.0));

	return output;
}