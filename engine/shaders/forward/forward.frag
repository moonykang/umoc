#include "../common.hlsl"

struct VSOutput
{
    float4 pos : SV_POSITION;
    [[vk::location(0)]] float3 normal : NORMAL;
    [[vk::location(1)]] float3 color : COLOR0;
    [[vk::location(2)]] float3 view : TEXCOORD1;
    [[vk::location(3)]] float3 light : TEXCOORD2;
    [[vk::location(4)]] float4 shadowUv : TEXCOORD3;
};


[[vk::binding(0, 2)]] Texture2D shadowDepthTexture;
[[vk::binding(0, 2)]] SamplerState shadowDepthSampler;

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
    //float shadow = textureProj(input.shadowUv / input.shadowUv.w, float2(0.f, 0.f));
    float shadow = filterPCF(input.shadowUv / input.shadowUv.w);

    float3 N = normalize(input.normal);
    float3 L = normalize(input.light);
    float3 V = normalize(input.view);
    float3 R = normalize(-reflect(L, N));
    float3 diffuse = max(dot(N, L), ambient) * input.color;

    float3 outColor = diffuse * shadow;

    return float4(outColor, 1.0f);
}
