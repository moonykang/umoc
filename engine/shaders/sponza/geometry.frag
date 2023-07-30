// Copyright 2020 Google LLC
#include "../common.hlsl"

struct VSOutput
{
	float4 pos : SV_POSITION;
    [[vk::location(0)]] float3 viewPos : POSITION0;
    [[vk::location(1)]] float3 normal : NORMAL0;
    [[vk::location(2)]] float2 uv : TEXCOORD0;
};

struct FSOutput
{
	float4 pos : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float4 albedo : SV_TARGET2;
};

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneUBO;
}

[[vk::binding(1, 2)]] Texture2D albedoMapTexture;
[[vk::binding(1, 2)]] SamplerState albedoMapSampler;
[[vk::binding(2, 2)]] Texture2D roughnessMetallicTexture;
[[vk::binding(2, 2)]] SamplerState roughnessMetallicSampler;

float linearDepth(float depth)
{
	float z = depth * 2.0f - 1.0f;
	return (2.0f * sceneUBO.nearPlane * sceneUBO.farPlane) / (sceneUBO.farPlane + sceneUBO.nearPlane - z * (sceneUBO.farPlane - sceneUBO.nearPlane));
}

FSOutput main(VSOutput input)
{
	FSOutput output = (FSOutput)0;

	float4 roughnessMetallic = roughnessMetallicTexture.Sample(roughnessMetallicSampler, input.uv);

	output.pos = float4(input.viewPos, linearDepth(input.pos.z));
	output.normal = float4(normalize(input.normal) * 0.5 + 0.5, roughnessMetallic.g);
	output.albedo = float4(albedoMapTexture.Sample(albedoMapSampler, input.uv).xyz, roughnessMetallic.b);

	return output;
}