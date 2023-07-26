// Copyright 2020 Google LLC
#include "../common.hlsl"

struct VSInput
{
	[[vk::location(0)]] float4 Pos : POSITION0;
	[[vk::location(1)]] float4 Vel : TEXCOORD0;
};

struct VSOutput
{
	float4 Pos : SV_POSITION;
	[[vk::location(0)]] float GradientPos : POSITION0;
	[[vk::location(1)]] float2 CenterPos : POSITION1;
	[[vk::builtin("PointSize")]] float PSize : PSIZE;
	[[vk::location(2)]] float PointSize : TEXCOORD0;
};

[[vk::binding(0, 0)]] cbuffer ubo : register(b2) { SceneView sceneView; }


[[vk::binding(0, 1)]] cbuffer ubo
{
    Model modelUBO;
}

struct PushConsts
{
  float2 screendim;
};

[[vk::push_constant]] PushConsts pushConstants;

VSOutput main (VSInput input)
{
	VSOutput output = (VSOutput)0;
	const float spriteSize = 0.005 * input.Pos.w; // Point size influenced by mass (stored in input.Pos.w);

	float4x4 modelView = mul(modelUBO.transform, sceneView.view);

	float4 eyePos = mul(modelView, float4(input.Pos.x, input.Pos.y, input.Pos.z, 1.0));
	float4 projectedCorner = mul(sceneView.projection, float4(0.5 * spriteSize, 0.5 * spriteSize, eyePos.z, eyePos.w));
	output.PSize = output.PointSize = clamp(pushConstants.screendim.x * projectedCorner.x / projectedCorner.w, 1.0, 128.0);

	output.Pos = mul(sceneView.projection, eyePos);
	output.CenterPos = ((output.Pos.xy / output.Pos.w) + 1.0) * 0.5 * pushConstants.screendim;

	output.GradientPos = input.Vel.w;
	return output;
}