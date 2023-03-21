#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUBO_1
{
    mat4  view_inverse;
    mat4  proj_inverse;
    mat4  view_proj_inverse;
    mat4  prev_view_proj;
    mat4  view_proj;
    vec4  cam_pos;
} sceneView;

layout(set = 0, binding = 1) uniform GlobalUBO_2
{
    vec4  lightPosition;
} sceneLight;

layout(set = 2, binding = 0) uniform ubo_material
{
    vec4 materialFactors; // x: alphaCutoff, y: metallicFactor, z: roughnessFactor, w: reserved
	vec4 baseColorFactor;
} materialUBO;

layout (set = 2, binding = 1) uniform sampler2D textureAlbedo;



const float PI = 3.14159265359;

// Normal Distribution function --------------------------------------
float D_GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
	return (alpha2)/(PI * denom*denom); 
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
vec3 F_Schlick(vec3 albedo, float cosTheta, float metallic)
{
	vec3 F0 = mix(vec3(0.04), albedo, metallic);
	vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0); 
	return F;    
}

// Specular BRDF composition --------------------------------------------
vec3 BRDF(vec3 L, vec3 V, vec3 N, vec3 albedo, float metallic, float roughness)
{
	// Precalculate vectors and dot products	
	vec3 H = normalize (V + L);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);
	float dotLH = clamp(dot(L, H), 0.0, 1.0);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);

	// Light color fixed
	vec3 lightColor = vec3(1.0);

	vec3 color = vec3(0.0);

	if (dotNL > 0.0)
	{
		float rroughness = max(0.05, roughness);
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness); 
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, rroughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		vec3 F = F_Schlick(albedo, dotNV, metallic);

		vec3 spec = D * F * G / (4.0 * dotNL * dotNV);

		color += spec * dotNL * lightColor;
	}

	return color;
}

void main() {
	vec3 N = normalize(inNormal);
	vec3 V = normalize(sceneView.cam_pos.xyz - inPos);

    float metallic = materialUBO.materialFactors.y;
	float roughness = materialUBO.materialFactors.z;

    vec4 albedo = texture(textureAlbedo, inUV);

	vec3 Lo = vec3(0.0);
	vec3 L = normalize(sceneLight.lightPosition.xyz - inPos);
	Lo += BRDF(L, V, N, albedo.xyz, metallic, roughness);

    vec3 color = albedo.xyz * 0.02;
    color += Lo;

	color = pow(color, vec3(0.4545));

	outColor = vec4(color, 1.0);
}