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
	float depth = sceneDepthTexture.Sample(sceneDepthSampler, input.uv).r;
	float3 fragPos = world_position_from_depth(input.uv, depth, sceneUBO.view_proj_inverse);

	float3 normal = gBufferBTexture.Sample(gBufferBSampler, input.uv).rgb;
	float4 albedo = gBufferCTexture.Sample(gBufferCSampler, input.uv);


    #define lightCount 6
	#define ambient 0.0

	// Ambient part
	float3 color = albedo.rgb * ambient;

    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        // Vector to light
		// float3 L = ubo.lights[i].position.xyz - fragPos;
        float3 L = light_position(lightUBO.lights[i]) - fragPos;

		// Distance from light to fragment position
		float dist = length(L);

		// Viewer to fragment
		float3 V = sceneUBO.pos.xyz - fragPos;
		V = normalize(V);

		float radius = light_radius(lightUBO.lights[i]);

		if(dist < radius)
		{
			// Light to fragment
			L = normalize(L);

			float3 lightColor = light_color(lightUBO.lights[i]);

			// Attenuation
			float atten = radius / (pow(dist, 2.0) + 1.0);

			// Diffuse part
			float3 N = normalize(normal);
			float NdotL = max(0.0, dot(N, L));
			float3 diff = lightColor * albedo.rgb * NdotL * atten;

			// Specular part
			// Specular map values are stored in alpha of albedo mrt
			float3 R = reflect(-L, N);
			float NdotR = max(0.0, dot(R, V));
			float3 spec = lightColor * albedo.a * pow(NdotR, 16.0) * atten;

			color += diff + spec;
		}

    }

    return float4(color, 1.0f);
}