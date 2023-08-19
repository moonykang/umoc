// Copyright 2020 Google LLC

#include "../common.hlsl"

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneView;
}

[[vk::binding(1, 0)]] cbuffer ubo
{
    SceneLight sceneLight;
}

[[vk::binding(0, 1)]] cbuffer ubo
{
    Model model;
}

float4 main([[vk::location(0)]] float3 Pos : POSITION0) : SV_POSITION
{
	float4x4 depthMVP = mul(sceneLight.lightMatrix, model.transform);
	return mul(depthMVP, float4(Pos, 1.0));
}