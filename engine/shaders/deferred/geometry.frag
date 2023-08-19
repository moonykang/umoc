// Copyright 2020 Google LLC
struct VSOutput
{
	float4 pos : SV_POSITION;
    [[vk::location(0)]] float3 worldPos : POSITION0;
    [[vk::location(1)]] float3 normal : NORMAL0;
    [[vk::location(2)]] float2 uv : TEXCOORD0;
    [[vk::location(3)]] float3 tangent : TEXCOORD1;
};

struct FSOutput
{
	float4 gBufferA : SV_TARGET0;
	float4 gBufferB : SV_TARGET1;
	float4 gBufferC : SV_TARGET2;
};

[[vk::binding(1, 2)]] Texture2D albedoMapTexture;
[[vk::binding(1, 2)]] SamplerState albedoMapSampler;
[[vk::binding(2, 2)]] Texture2D roughnessMetallicTexture;
[[vk::binding(2, 2)]] SamplerState roughnessMetallicSampler;

float compute_curvature(float3 normal, float depth)
{
    float3 dx = ddx(normal);
    float3 dy = ddy(normal);

    float x = dot(dx, dx);
    float y = dot(dy, dy);

    return pow(max(x, y), 0.5f);
}

FSOutput main(VSOutput input)
{
	FSOutput output = (FSOutput)0;

    float4 albedo = albedoMapTexture.Sample(albedoMapSampler, input.uv);
	float4 roughnessMetallic = roughnessMetallicTexture.Sample(roughnessMetallicSampler, input.uv);

	float roughness = roughnessMetallic.g;
	float metallic = roughnessMetallic.b;
	float linear_z  = input.pos.z / input.pos.w;
    float curvature = compute_curvature(input.normal, linear_z);

	output.gBufferA = float4(albedo.xyz, metallic);
	output.gBufferB = float4(normalize(input.normal) * 0.5 + 0.5, 1.0);
	//output.gBufferC = float4(roughness, linear_z, curvature, 1.0f);
	output.gBufferC = float4(roughness, input.pos.x, input.pos.y, input.pos.z);

	return output;
}