#ifndef BRDF_HLSL
#define BRDF_HLSL

#include "common.hlsl"

// ------------------------------------------------------------------------

// ------------------------------------------------------------------------

float DistributionGGX(in float ndoth, in float alpha)
{
    float a2    = alpha * alpha;
    float denom = (ndoth * ndoth) * (a2 - 1.0) + 1.0;

    return a2 / max(EPSILON, (MPI * denom * denom));
}

// ------------------------------------------------------------------------

float G1_schlick_ggx(in float roughness, in float ndotv)
{
    float k = ((roughness + 1) * (roughness + 1)) / 8.0;

    return ndotv / max(EPSILON, (ndotv * (1 - k) + k));
}

// ------------------------------------------------------------------------

float GeometrySchlickGGX(in float ndotl, in float ndotv, in float roughness)
{
    return G1_schlick_ggx(roughness, ndotl) * G1_schlick_ggx(roughness, ndotv);
}

// ------------------------------------------------------------------------

float3 FresnelSchlick(in float3 f0, in float vdoth)
{
    return f0 + (float3(1.0, 1.0, 1.0) - f0) * (pow(1.0 - vdoth, 5.0));
}

// ------------------------------------------------------------------------

float3 brdf_cook_torrance(in float roughness, in float3 F, in float ndoth, in float ndotl, in float ndotv)
{
    float alpha = roughness * roughness;
    return (DistributionGGX(ndoth, alpha) * F * GeometrySchlickGGX(ndotl, ndotv, roughness)) / max(EPSILON, (4.0 * ndotl * ndotv));
}

// ------------------------------------------------------------------------

float3 brdf_labert(in float3 diffuse_color)
{
    return diffuse_color / MPI;
}

float3 reflection_brdf(in float3 diffuse_color, 
                       in float roughness, 
                       in float3 N, 
                       in float3 F0, 
                       in float3 Wo, 
                       in float3 Wh, 
                       in float3 Wi)
{
    float NdotL = max(dot(N, Wi), 0.0);
    float NdotV = max(dot(N, Wo), 0.0);
    float NdotH = max(dot(N, Wh), 0.0);
    float VdotH = max(dot(Wi, Wh), 0.0);

    float3 F        = FresnelSchlick(F0, VdotH);
    float3 specular = brdf_cook_torrance(roughness, F, NdotH, NdotL, NdotV);
    float3 diffuse  = brdf_labert(diffuse_color.xyz);

    return (float3(1.0, 1.0, 1.0) - F) * diffuse + specular;
}
#endif