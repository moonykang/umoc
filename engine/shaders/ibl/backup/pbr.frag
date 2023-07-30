#include "../common.hlsl"

struct VSOutput
{
    float4 pos : SV_POSITION;
    [[vk::location(0)]] float3 worldPos : POSITION;
    [[vk::location(1)]] float3 normal : NORMAL;
    [[vk::location(2)]] float2 uv : TEXCOORD;
};

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneView;
}

[[vk::binding(1, 0)]] cbuffer ubo
{
    SceneLight sceneLight;
}

struct PushConsts {
    float roughness;
    float metallic;
    float specular;
    float r;
    float g;
    float b;
};
[[vk::push_constant]] PushConsts material;

// ----------------------------------------------------------------------------
float DistributionGGX(float3 N, float3 H, float roughness)
{
    const float MPI = 3.14159265359f;

    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = MPI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------

float4 main(VSOutput input) : SV_TARGET
{
    const float MPI = 3.14159265359f;

    float3 N = input.normal;
    float3 V = normalize(sceneView.pos.xyz - input.worldPos);
    float3 R = reflect(-V, N);

    float3 albedo = float3(material.r, material.g, material.b);
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = mix(F0, albedo, material.metallic);

    float3 Lo = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; i++)
    {
        float3 L = normalize(sceneLight.lights[i].pos.xyz - input.worldPos);
        float3 H = normalize(V + L);
        float distance = length(sceneLight.lights[i].pos.xyz - input.worldPos);
        float attenuation = 1.0f / (distance * distance);
        float3 radiance = sceneLight.lights[i].color * attenuation;

        float NDF = DistributionGGX(N, H, material.roughness);
        float G = GeometrySmith(N, V, L, material.roughness);
        float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        float3 numerator = NDF * G * F;
        float denominator = 4.0f * max(dot(N,V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f;
        float3 specular = numerator / denominator;

        float3 kS = F;
        float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;

        kD *= 1.0f - material.metallic;

        float NdotL = max(dot(N, L), 0.0f);

        Lo += (kD * albedo/ MPI + specular) * radiance * NdotL;
    }

    float3 ambient = float3(0.03f, 0.03f, 0.03f) * albedo;
    float3 color = ambient + Lo;

    color = color / (color + float3(1.0f, 1.0f, 1.0f));

    float gamma = 1.0 / 2.2f;
    color = pow(color, float3(gamma, gamma, gamma));

    return float4(color, 1.0f);
}