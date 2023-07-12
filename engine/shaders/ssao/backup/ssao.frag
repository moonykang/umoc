#include "../common.hlsl"

struct VSOutput
{
    [[vk::location(0)]] float2 uv : TEXCOORD;
};

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneUBO;
}

[[vk::binding(1, 0)]] cbuffer ubo
{
    SceneLight lightUBO;
}

#define SSAO_KERNEL_ARRAY_SIZE 64

struct SSAOKernelUBO
{
	float4 samples[SSAO_KERNEL_ARRAY_SIZE];
    uint kernel_size;
    float radius;
};

[[vk::binding(0, 1)]] cbuffer ubo
{
    SSAOKernelUBO ssaoKernelUBO;
}

[[vk::binding(1, 1)]] Texture2D gBufferATexture; // pos
[[vk::binding(1, 1)]] SamplerState gBufferASampler;

[[vk::binding(2, 1)]] Texture2D gBufferBTexture; // normal
[[vk::binding(2, 1)]] SamplerState gBufferBSampler;

[[vk::binding(3, 1)]] Texture2D ssaoNoiseTexture; // SSAO Noise texture
[[vk::binding(3, 1)]] SamplerState ssaoNoiseSampler;

float main(VSOutput input) : SV_TARGET
{
	float3 fragPos = gBufferATexture.Sample(gBufferASampler, input.uv).rgb;
	float3 normal = normalize(gBufferBTexture.Sample(gBufferBSampler, input.uv).rgb * 2.0 - 1.0);
	float4 noise = ssaoNoiseTexture.Sample(ssaoNoiseSampler, input.uv);

    // Get a random vector using a noise lookup
	int2 texDim;
	gBufferATexture.GetDimensions(texDim.x, texDim.y);
	int2 noiseDim;
	ssaoNoiseTexture.GetDimensions(noiseDim.x, noiseDim.y);

	const float2 noiseUV = float2(float(texDim.x)/float(noiseDim.x), float(texDim.y)/(noiseDim.y)) * input.uv;
	float3 randomVec = ssaoNoiseTexture.Sample(ssaoNoiseSampler, noiseUV).xyz * 2.0 - 1.0;

	// Create TBN matrix
	float3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	float3 bitangent = cross(tangent, normal);
	float3x3 TBN = transpose(float3x3(tangent, bitangent, normal));

	// Calculate occlusion value
	float occlusion = 0.0f;
	for(int i = 0; i < ssaoKernelUBO.kernel_size; i++)
	{
		float3 samplePos = mul(TBN, ssaoKernelUBO.samples[i].xyz);
		samplePos = fragPos + samplePos * ssaoKernelUBO.radius;

		// project
		float4 offset = float4(samplePos, 1.0f);
		offset = mul(sceneUBO.proj, offset);
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;

		float sampleDepth = -gBufferATexture.Sample(gBufferASampler, offset.xy).w;

		float rangeCheck = smoothstep(0.0f, 1.0f, ssaoKernelUBO.radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z ? 1.0f : 0.0f) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / float(ssaoKernelUBO.kernel_size));

	return occlusion;
}