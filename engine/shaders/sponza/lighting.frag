#include "../common.hlsl"
#include "../lighting.hlsl"

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


[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneView;
}

[[vk::binding(1, 0)]] cbuffer ubo
{
    SceneLight sceneLight;
}

//static const float MPI = 3.14159265359;

// Normal Distribution function --------------------------------------
float D_GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
	return (alpha2)/(MPI * denom*denom);
}

// Geometric Shadowing function --------------------------------------
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float GL = dotNL / (dotNL * (1.0 - k) + k);
	float GV = dotNV / (dotNV * (1.0 - k) + k);
	return GL * GV;
}

// Fresnel function ----------------------------------------------------
float3 F_Schlick(float cosTheta, float metallic, float3 albedo)
{
	float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic); // * material.specular
	float3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
	return F;
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
// Specular BRDF composition --------------------------------------------

float3 BRDF(float3 L, float3 V, float3 N, float metallic, float roughness, float3 albedo)
{
	// Precalculate vectors and dot products
	float3 H = normalize (V + L);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);
	float dotLH = clamp(dot(L, H), 0.0, 1.0);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);

	// Light color fixed
	float3 lightColor = float3(1.0, 1.0, 1.0);

	float3 color = float3(0.0, 0.0, 0.0);

	if (dotNL > 0.0)
	{
		float rroughness = max(0.05, roughness);
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness);
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, rroughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		float3 F = F_Schlick(dotNV, metallic, albedo);

		float3 spec = D * F * G / (4.0 * dotNL * dotNV);

		color += spec * dotNL * lightColor;
	}

	return color;
}

float3 BRDF(float3 L, float3 V, float3 N, float3 F0, float3 lightColor, float3 albedo, float metallic, float roughness)
{
	// Precalculate vectors and dot products
	float3 H = normalize (V + L);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);

	// Light color fixed
	//float3 lightColor = float3(1.0, 1.0, 1.0);

	float3 color = float3(0.0, 0.0, 0.0);

	if (dotNL > 0.0) {
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness);
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		float3 F = F_Schlick(dotNV, F0);
		float3 spec = D * F * G / (4.0 * dotNL * dotNV + 0.001);

		float3 kS = F;
		float3 kD = (float3(1.0, 1.0, 1.0) - kS) * (1.0 - metallic);

		color += (kD * albedo / MPI + spec) * dotNL * lightColor;
	}

	return color;
}

float4 main(VSOutput input) : SV_TARGET
{
    // Get G-Buffer values
	float4 gBufferA = gBufferATexture.Sample(gBufferASampler, input.uv);
	float3 fragPos = gBufferA.xyz;

	float4 gBufferB = gBufferBTexture.Sample(gBufferBSampler, input.uv);
	float3 normal = gBufferB.xyz;
	float roughness = gBufferB.w;

	float4 gBufferC = gBufferCTexture.Sample(gBufferCSampler, input.uv);
	float3 albedo = gBufferC.xyz;
	float metallic = gBufferC.w;

    #define lightCount 6
	#define ambient 0.01

	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, albedo, metallic);

	float3 N = normalize(normal);
	float3 V = normalize(sceneView.pos.xyz - fragPos);
	float3 R = reflect(-V, N);

	float3 Lo = float3(0.0f, 0.0f, 0.0f);

	// Ambient part
	float3 color = albedo.rgb * ambient;

    for (int i = 1; i < sceneLight.numLights; i++)
    {
        float3 Lo = direct_lighting(sceneLight.lights[i], V, N, fragPos, F0, albedo, roughness);
		color += Lo;
    }

	color = color / (color + float3(1.0f, 1.0f, 1.0f));

	float gamma = 1.0f / 2.2f;
	color = pow(color, float3(gamma, gamma, gamma));

    return float4(color, 1.0f);
}