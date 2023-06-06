#version 450

layout (location = 0) in vec3 inWorldPos;
layout (location = 1) in vec3 inNormal;

layout (location = 0) out vec4 outColor;

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
	float gamma;
	float exposure;
} sceneLight;

layout(push_constant) uniform PushConsts {
	float roughness;
	float metallic;
    float specular;
	float r;
	float g;
	float b;
} material;

const float PI = 3.14159265359;

vec3 materialcolor()
{
	return vec3(material.r, material.g, material.b);
}

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
vec3 F_Schlick(float cosTheta, float metallic)
{
	vec3 F0 = mix(vec3(0.04), materialcolor(), metallic); // * material.specular
	vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0); 
	return F;    
}

vec3 BRDF(vec3 albedo, vec3 L, vec3 V, vec3 N, float metallic, float roughness)
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
		vec3 F = F_Schlick(dotNV, metallic);

        vec3 kS = F;
        vec3 kD = vec3(1.0f) - kS;
        kD *= 1.0 - metallic;

        vec3 numerator = D * G * F;
        float denominator = 4.0 * dotNV * dotNL + 0.0001;
        vec3 specular = numerator / denominator;

        color += (kD * albedo / PI + specular) * dotNL * lightColor;
		// vec3 spec = D * F * G / (4.0 * dotNL * dotNV);

		//color += spec * dotNL * lightColor;
	}

	return color;
}

void main()
{
    vec3 albedo = materialcolor();

    vec3 L = normalize(sceneLight.lightPosition.xyz - inWorldPos);
    vec3 N = normalize(inNormal);
    vec3 V = normalize(sceneView.cam_pos.xyz - inWorldPos);
    vec3 H = normalize(L + V);

    vec3 Lo = vec3(0.0f);
    
    Lo += BRDF(albedo, L, V, N, material.metallic, material.roughness);

    vec3 color = albedo * 0.2f;
    color += Lo;

    outColor = vec4(color, 1.0f);
}