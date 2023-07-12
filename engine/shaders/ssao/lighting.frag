#include "../common.hlsl"

struct VSOutput
{
    [[vk::location(0)]] float2 uv : TEXCOORD;
};

[[vk::binding(0, 1)]] Texture2D gBufferATexture; // pos
[[vk::binding(0, 1)]] SamplerState gBufferASampler;

[[vk::binding(1, 1)]] Texture2D gBufferBTexture; // normal
[[vk::binding(1, 1)]] SamplerState gBufferBSampler;

[[vk::binding(2, 1)]] Texture2D gBufferCTexture; // albedo
[[vk::binding(2, 1)]] SamplerState gBufferCSampler;

[[vk::binding(3, 1)]] Texture2D ssaoTexture;
[[vk::binding(3, 1)]] SamplerState ssaoSampler;

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
    	// Get G-Buffer values
	float3 fragPos = gBufferATexture.Sample(gBufferASampler, input.uv).rgb;
	float3 normal = gBufferBTexture.Sample(gBufferBSampler, input.uv).rgb;
	float4 albedo = gBufferCTexture.Sample(gBufferCSampler, input.uv);
    float ao = ssaoTexture.Sample(ssaoSampler, input.uv).r;

    #define lightCount 6
	#define ambient 0.3

	// Ambient part
	float3 color = albedo.rgb * ambient * ao;
/*
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        // Vector to light
		// float3 L = ubo.lights[i].position.xyz - fragPos;
        float3 L = lightUBO.lights[i].pos.xyz - fragPos;

		// Distance from light to fragment position
		float dist = length(L);

		// Viewer to fragment
		float3 V = sceneUBO.pos.xyz - fragPos;
		V = normalize(V);

		if(dist < lightUBO.lights[i].radius)
		{
			// Light to fragment
			L = normalize(L);

			// Attenuation
			float atten = lightUBO.lights[i].radius / (pow(dist, 2.0) + 1.0);

			// Diffuse part
			float3 N = normalize(normal);
			float NdotL = max(0.0, dot(N, L));
			float3 diff = lightUBO.lights[i].color * albedo.rgb * NdotL * atten;

			// Specular part
			// Specular map values are stored in alpha of albedo mrt
			float3 R = reflect(-L, N);
			float NdotR = max(0.0, dot(R, V));
			float3 spec = lightUBO.lights[i].color * albedo.a * pow(NdotR, 16.0) * atten;

			color += diff + spec;
		}

    }
*/
    return float4(color, 1.0f);
}