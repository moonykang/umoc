#include "../common.hlsl"

struct VSOutput
{
    float4 pos : SV_POSITION;
    [[vk::location(0)]] float3 worldPos : POSITION;
    [[vk::location(1)]] float3 normal : NORMAL;
    [[vk::location(2)]] float2 uv : TEXCOORD;
    [[vk::location(3)]] float3 color : COLOR0;
    [[vk::location(4)]] float4 shadowUv : TEXCOORD3;
};


[[vk::binding(0, 2)]] Texture2D albedoTexture;
[[vk::binding(0, 2)]] SamplerState albedoSampler;
[[vk::binding(1, 2)]] Texture2D shadowDepthTexture;
[[vk::binding(1, 2)]] SamplerState shadowDepthSampler;

#define ambient 0.1

float textureProj(float4 shadowCoord, float2 off)
{
	float shadow = 1.0;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 )
	{
		float dist = shadowDepthTexture.Sample(shadowDepthSampler, shadowCoord.xy + off ).r;
		if ( shadowCoord.w > 0.0 && dist < shadowCoord.z )
		{
			shadow = ambient;
		}
	}
	return shadow;
}

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneView;
}

[[vk::binding(1, 0)]] cbuffer ubo
{
    SceneLight sceneLight;
}

float filterPCF(float4 sc)
{
	int2 texDim;
	shadowDepthTexture.GetDimensions(texDim.x, texDim.y);
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;

	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += textureProj(sc, float2(dx*x, dy*y));
			count++;
		}

	}
	return shadowFactor / count;
}

#define BLIN_PHONG 1

float4 main(VSOutput input) : SV_TARGET
{
	float3 albedo = albedoTexture.Sample(albedoSampler, input.uv).xyz;

	float4 shadowPos = input.shadowUv / input.shadowUv.w;
    //float shadow = textureProj(shadowPos, float2(0.f, 0.f));
    float shadow = filterPCF(shadowPos);

    float3 N = normalize(input.normal);
	float3 L = normalize(light_position(sceneLight.lights[0]) - input.worldPos);
	float3 V = normalize(sceneView.pos.xyz - input.worldPos);

#if BLIN_PHONG
	float3 H = normalize(L + V);
	float spec = pow(max(dot(N, H), 0.0f), 32.0f);
#else
	float3 R = reflect(-L, N);
	float spec = pow(max(dot(V, R), 0.0f), 8.0f);
#endif

	float3 S = float3(0.3f, 0.3f, 0.3f) * spec;
	float3 D = max(dot(L, N), 0.0f);
	float3 A = albedo * ambient;

	float3 color = (A + D + S) * shadow;

    //return float4(color, 1.0f);

	float dist = shadowDepthTexture.Sample(shadowDepthSampler, shadowPos.xy).r;
	return float4(shadowPos.xyz, dist);
}
