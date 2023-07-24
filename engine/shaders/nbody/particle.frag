// Copyright 2020 Google LLC

[[vk::binding(0, 1)]] Texture2D textureColorMap;
[[vk::binding(0, 1)]] SamplerState samplerColorMap;
[[vk::binding(1, 1)]] Texture2D textureGradientRamp;
[[vk::binding(1, 1)]] SamplerState samplerGradientRamp;

struct VSOutput
{
	float4 Pos : SV_POSITION;
[[vk::location(0)]] float GradientPos : POSITION0;
[[vk::location(1)]] float2 CenterPos : POSITION1;
[[vk::location(2)]] float PointSize : TEXCOORD0;
};

float4 main (VSOutput input)  : SV_TARGET
{
	float3 color = textureGradientRamp.Sample(samplerGradientRamp, float2(input.GradientPos, 0.0)).rgb;
	float2 PointCoord = (input.Pos.xy - input.CenterPos.xy) / input.PointSize + 0.5;
	return float4(textureColorMap.Sample(samplerColorMap, PointCoord).rgb * color, 1);
}
