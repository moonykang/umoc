#ifndef BRDF_H
#define BRDF_H

// ------------------------------------------------------------------------

float D_ggx(in float ndoth, in float alpha)
{
    float a2    = alpha * alpha;
    float denom = (ndoth * ndoth) * (a2 - 1.0) + 1.0;

    return a2 / max(EPSILON, (PI * denom * denom));
}

// ------------------------------------------------------------------------

// ------------------------------------------------------------------------

float G_schlick_ggx(in float dotNL, in float dotNV, in float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;
	float GL = dotNL / (dotNL * (1.0 - k) + k);
	float GV = dotNV / (dotNV * (1.0 - k) + k);

    return GL * GV;
}

// ------------------------------------------------------------------------
float3 FresnelSchlick(in float3 F0, in float dotVH)
{
    return F0 + (1.0 - F0) * (pow(1 - dotVH, 5.0));
}

// ------------------------------------------------------------------------
float3 specular_brdf(in float roughness, in float3 F, in float ndoth, in float ndotl, in float ndotv)
{
    float alpha = roughness * roughness;
    return (D_ggx(ndoth, alpha) * F * G_schlick_ggx(ndotl, ndotv, roughness)) / max(EPSILON, (4.0 * ndotl * ndotv));
}
// ------------------------------------------------------------------------

float3 diffuse_brdf(in float3 diffuse)
{
    return diffuse / PI;
}
// ------------------------------------------------------------------------
float3 calculate_brdf(in float3 diffuse_color,
                      in float roughness,
                      in float3 F0,
                      in float3 N,
                      in float3 L,
                      in float3 V,
                      in float3 H)
{
    float dotNL = max(dot(N, L), 0.0f);
    float dotNV = max(dot(N, V), 0.0f);
    float dotNH = max(dot(N, H), 0.0f);
    float dotVH = max(dot(V, H), 0.0f);

    float3 F = FresnelSchlick(F0, dotVH);

	float3 color = float3(0.0, 0.0, 0.0);

    float3 specular = specular_brdf(roughness, F, dotNH, dotNL, dotNV);
    float3 diffuse  = diffuse_brdf(diffuse_color);

    return (1.0f - F) * diffuse + specular;
}
#endif // BRDF_H