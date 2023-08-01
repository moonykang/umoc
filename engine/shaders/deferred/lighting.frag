#include "../common.hlsl"

#define USE_PHONG 1
#include "../light.hlsl"

struct VSOutput
{
    [[vk::location(0)]] float2 uv : TEXCOORD;
};

[[vk::binding(0, 1)]] Texture2D gBufferATexture;
[[vk::binding(0, 1)]] SamplerState gBufferASampler;

[[vk::binding(1, 1)]] Texture2D gBufferBTexture;
[[vk::binding(1, 1)]] SamplerState gBufferBSampler;

[[vk::binding(2, 1)]] Texture2D gBufferCTexture;
[[vk::binding(2, 1)]] SamplerState gBufferCSampler;

[[vk::binding(3, 1)]] Texture2D sceneDepthTexture; // albedo
[[vk::binding(3, 1)]] SamplerState sceneDepthSampler;

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneUBO;
}

[[vk::binding(1, 0)]] cbuffer ubo
{
    SceneLight lightUBO;
}

float4 main(VSOutput input) : SV_TARGET
{
	float4 gBufferA = gBufferATexture.Sample(gBufferASampler, input.uv);
	float4 gBufferB = gBufferBTexture.Sample(gBufferBSampler, input.uv);
	float4 gBufferC = gBufferCTexture.Sample(gBufferCSampler, input.uv);
	float depth = sceneDepthTexture.Sample(sceneDepthSampler, input.uv).r;

	float3 fragPos = world_position_from_depth(input.uv, depth, sceneUBO.view_proj_inverse);

	float3 normal = gBufferBTexture.Sample(gBufferBSampler, input.uv).rgb;
	float3 albedo = gBufferA.xyz;

	float roughness = gBufferC.x;
	float metallic = gBufferA.w;

    #define lightCount 6
	#define ambient 0.0

	// Ambient part
	float3 color = albedo * ambient;
	float3 V = sceneUBO.pos.xyz - fragPos;
	V = normalize(V);

    const float3 F0        = mix(float3(0.04f, 0.04f, 0.04f), albedo, metallic);
    const float3 diffuse = mix(albedo * (1.0f - F0), float3(0.0f, 0.0f, 0.0f), metallic);

	float3 N = normalize(normal);

    for (int i = 0; i < lightUBO.numLights; i++)
    {
		float3 Lo = direct_lighting(lightUBO.lights[i], V, N, fragPos, F0, diffuse, roughness);
		color += Lo;
    }

    return float4(color, 1.0f);
}