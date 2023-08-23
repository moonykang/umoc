#include "../common.hlsl"

#define USE_PHONG 0
#include "../light.hlsl"

struct VSOutput
{
    [[vk::location(0)]] float2 uv : TEXCOORD;
};

[[vk::binding(0, 1)]] Texture2D gBufferATexture; // xyz: albedo, w: metallic
[[vk::binding(0, 1)]] SamplerState gBufferASampler;

[[vk::binding(1, 1)]] Texture2D gBufferBTexture; // xyz: normal
[[vk::binding(1, 1)]] SamplerState gBufferBSampler;

[[vk::binding(2, 1)]] Texture2D gBufferCTexture; // x: roughness, y: linear_z, z: culvature
[[vk::binding(2, 1)]] SamplerState gBufferCSampler;

[[vk::binding(3, 1)]] Texture2D sceneDepthTexture;
[[vk::binding(3, 1)]] SamplerState sceneDepthSampler;

[[vk::binding(4, 1)]] Texture2D shadowDepthTexture;
[[vk::binding(4, 1)]] SamplerState shadowDepthSampler;

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneUBO;
}

[[vk::binding(1, 0)]] cbuffer ubo
{
    SceneLight lightUBO;
}

static const float4x4 biasMat = float4x4(
	0.5, 0.0, 0.0, 0.5,
	0.0, 0.5, 0.0, 0.5,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0 );


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

float4 main(VSOutput input) : SV_TARGET
{
	float4 gBufferA = gBufferATexture.Sample(gBufferASampler, input.uv);
	float4 gBufferB = gBufferBTexture.Sample(gBufferBSampler, input.uv);
	float4 gBufferC = gBufferCTexture.Sample(gBufferCSampler, input.uv);
	float depth = sceneDepthTexture.Sample(sceneDepthSampler, input.uv).r;


	float3 fragPos = world_position_from_depth(input.uv, depth, sceneUBO.view_proj_inverse);

	float4 shadowUV = mul(biasMat, mul(lightUBO.lightMatrix, float4(fragPos, 1.0f)));
	float visibility = filterPCF(shadowUV);

	float3 normal = gBufferBTexture.Sample(gBufferBSampler, input.uv).rgb;
	float3 albedo = gBufferA.xyz;

	float roughness = gBufferC.x;
	float metallic = gBufferA.w;

	// Ambient part
	float3 color = albedo * ambient;
	float3 V = sceneUBO.pos.xyz - fragPos;
	V = normalize(V);

    const float3 F0        = mix(float3(0.04f, 0.04f, 0.04f), albedo, metallic);
    const float3 diffuse = mix(albedo * (1.0f - F0), float3(0.0f, 0.0f, 0.0f), metallic);

	float3 N = normalize(normal);

    for (int i = 0; i < lightUBO.numLights; i++)
    {
		float3 Lo;
		if (i == 0)
		{
		 	Lo = direct_lighting(lightUBO.lights[i], V, N, fragPos, F0, diffuse, roughness) * visibility;
		}
		else
		{
			Lo = direct_lighting(lightUBO.lights[i], V, N, fragPos, F0, diffuse, roughness);
		}
		color += Lo;
    }

    return float4(color, 1.0f);
}