// Copyright 2020 Google LLC

#include "../common.hlsl"


[[vk::binding(1, 0)]] cbuffer ubo
{
    SceneLight lightUBO;
}

float4 main([[vk::location(0)]] float3 Pos : POSITION0) : SV_POSITION
{
	float4x4 shadowMVP = mul(lightUBO.projection, lightUBO.view);
	return mul(shadowMVP, float4(Pos, 1.0));
}