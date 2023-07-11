// Copyright 2020 Google LLC

struct VSOutput
{
    [[vk::location(0)]] float3 normal : NORMAL0;
    [[vk::location(1)]] float2 uv : TEXCOORD0;
    [[vk::location(2)]] float3 color : COLOR0;
    [[vk::location(3)]] float3 worldPos : POSITION0;
    [[vk::location(4)]] float3 tangent : TEXCOORD1;
};

struct FSOutput
{
	float4 pos : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float4 albedo : SV_TARGET2;
};

[[vk::binding(0, 2)]] Texture2D albedoMapTexture;
[[vk::binding(0, 2)]] SamplerState albedoMapSampler;
[[vk::binding(1, 2)]] Texture2D normalMapTexture;
[[vk::binding(1, 2)]] SamplerState normalMapSampler;


FSOutput main(VSOutput input)
{
	FSOutput output = (FSOutput)0;

	output.pos = float4(input.worldPos, 1.0);

    float3 normal = normalMapTexture.Sample(normalMapSampler, input.uv).xyz;
    float4 albedo = albedoMapTexture.Sample(albedoMapSampler, input.uv);

	// Calculate normal in tangent space
	float3 N = normalize(input.normal);
	float3 T = normalize(input.tangent);
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);

	float3 tnorm = mul(normalize(normal * 2.0 - float3(1.0, 1.0, 1.0)), TBN);
	output.normal = float4(tnorm, 1.0);
	output.albedo = albedo;

	return output;
}