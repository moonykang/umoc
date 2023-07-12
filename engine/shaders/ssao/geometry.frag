// Copyright 2020 Google LLC
struct VSOutput
{
    [[vk::location(0)]] float3 worldPos : POSITION0;
    [[vk::location(1)]] float3 normal : NORMAL0;
    [[vk::location(2)]] float2 uv : TEXCOORD0;
    [[vk::location(3)]] float3 tangent : TEXCOORD1;
};

struct FSOutput
{
	float4 pos : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float4 albedo : SV_TARGET2;
};

[[vk::binding(0, 2)]] Texture2D albedoMapTexture;
[[vk::binding(0, 2)]] SamplerState albedoMapSampler;

FSOutput main(VSOutput input)
{
	FSOutput output = (FSOutput)0;

	output.pos = float4(input.worldPos, 1.0);

    float4 albedo = albedoMapTexture.Sample(albedoMapSampler, input.uv);
	output.normal = float4(normalize(input.normal) * 0.5 + 0.5, 1.0);
	output.albedo = albedo;

	return output;
}