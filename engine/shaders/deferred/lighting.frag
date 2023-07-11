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


struct SceneUBO
{
    float4x4 view_inverse;
    float4x4 proj_inverse;
    float4x4 view_proj_inverse;
    float4x4 prev_view_proj;
    float4x4 view_proj;
    float4 view_pos;
};

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneUBO sceneUBO;
}

struct LightUBO
{
    float4 pos;
    float gamma;
    float exposure;
};

[[vk::binding(1, 0)]] cbuffer ubo
{
    LightUBO lightUBO;
}

float4 main(VSOutput input) : SV_TARGET
{
    	// Get G-Buffer values
	float3 fragPos = gBufferATexture.Sample(gBufferASampler, input.uv).rgb;
	float3 normal = gBufferBTexture.Sample(gBufferBSampler, input.uv).rgb;
	float4 albedo = gBufferCTexture.Sample(gBufferCSampler, input.uv);


    #define lightCount 6
	#define ambient 0.0

	// Ambient part
	float3 color = albedo.rgb * ambient;

        // Vector to light
		// float3 L = ubo.lights[i].position.xyz - fragPos;
        float3 L = lightUBO.pos.xyz - fragPos;

		// Distance from light to fragment position
		float dist = length(L);

		// Viewer to fragment
		float3 V = sceneUBO.view_pos.xyz - fragPos;
		V = normalize(V);

		//if(dist < ubo.lights[i].radius)
		{
			// Light to fragment
			L = normalize(L);

			// Attenuation
			//float atten = ubo.lights[i].radius / (pow(dist, 2.0) + 1.0);

			// Diffuse part
			float3 N = normalize(normal);
			float NdotL = max(0.0, dot(N, L));
			//float3 diff = ubo.lights[i].color * albedo.rgb * NdotL;// * atten;
            float3 diff = albedo.rgb * NdotL;// * atten;

			// Specular part
			// Specular map values are stored in alpha of albedo mrt
			float3 R = reflect(-L, N);
			float NdotR = max(0.0, dot(R, V));
			//float3 spec = ubo.lights[i].color * albedo.a * pow(NdotR, 16.0) * atten;
            float3 spec = albedo.a * pow(NdotR, 16.0);// * atten;

			color += diff + spec;
		}

    return float4(albedo.xyz, 1.0f);
}