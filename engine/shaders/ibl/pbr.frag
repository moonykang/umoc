// Copyright 2020 Google LLC

#include "../common.hlsl"
#include "../light.hlsl"

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

[[vk::binding(1, 0)]] cbuffer ubo
{
    SceneLight sceneLight;
}

struct PushConsts {
    float roughness;
    float metallic;
    float specular;
    float r;
    float g;
    float b;
};
[[vk::push_constant]] PushConsts material;

[[vk::binding(0, 2)]] TextureCube textureIrradiance;
[[vk::binding(0, 2)]] SamplerState samplerIrradiance;

[[vk::binding(1, 2)]] Texture2D textureBRDFLUT;
[[vk::binding(1, 2)]] SamplerState samplerBRDFLUT;

[[vk::binding(2, 2)]] TextureCube prefilteredMapTexture;
[[vk::binding(2, 2)]] SamplerState prefilteredMapSampler;

//#define ROUGHNESS_PATTERN 1

float3 materialcolor()
{
	return float3(material.r, material.g, material.b);
}

// Fresnel function ----------------------------------------------------
float3 F_Schlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
float3 F_SchlickR(float cosTheta, float3 F0, float roughness)
{
	return F0 + (max((1.0 - roughness).xxx, F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 prefilteredReflection(float3 R, float roughness)
{
	const float MAX_REFLECTION_LOD = 9.0; // todo: param/const
	float lod = roughness * MAX_REFLECTION_LOD;
	float lodf = floor(lod);
	float lodc = ceil(lod);
	float3 a = prefilteredMapTexture.SampleLevel(prefilteredMapSampler, R, lodf).rgb;
	float3 b = prefilteredMapTexture.SampleLevel(prefilteredMapSampler, R, lodc).rgb;
	return lerp(a, b, lod - lodf);
}

// ----------------------------------------------------------------------------
float4 main(VSOutput input) : SV_TARGET
{
	float3 albedo = materialcolor();
	float3 N = normalize(input.normal);
	float3 V = normalize(sceneView.pos.xyz - input.worldPos);
	float3 R = reflect(-V, N);

	float metallic = material.metallic;
	float roughness = material.roughness;

	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, albedo, metallic);
    const float3 d = mix(albedo * (1.0f - F0), float3(0.0f, 0.0f, 0.0f), metallic);

	float3 Lo = float3(0.0, 0.0, 0.0);
	
	for(int i = 0; i < sceneLight.numLights; i++) {
		Lo += direct_lighting(sceneLight.lights[i], V, N, input.worldPos, F0, d, roughness);
	}

	float2 brdf = textureBRDFLUT.Sample(samplerBRDFLUT, float2(max(dot(N, V), 0.0), roughness)).rg;
	float3 reflection = prefilteredReflection(R, roughness).rgb;
	float3 irradiance = textureIrradiance.Sample(samplerIrradiance, N).rgb;

	// Diffuse based on irradiance
	float3 diffuse = irradiance * albedo;

	float3 F = F_SchlickR(max(dot(N, V), 0.0), F0, roughness);

	// Specular reflectance
	float3 specular = reflection * (F * brdf.x + brdf.y);

	// Ambient part
	float3 kD = 1.0 - F;
	kD *= 1.0 - metallic;
	float3 ambient = (kD * diffuse + specular);

	float3 color = ambient + Lo;

	return float4(color, 1.0);
}