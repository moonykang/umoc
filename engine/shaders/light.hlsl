#ifndef LIGHT_H
#define LIGHT_H

#include "common.hlsl"
#include "brdf.hlsl"

void fetch_light_properties(
    in Light light,
    in float3 Wo,
    in float3 P,
    in float3 N,
    out float3 Li,
    out float3 L,
    out float3 H,
    out float attenuation
)
{
    const int type = light_type(light);

    Li = light_color(light) * light_intensity(light);

    if (type == LIGHT_TYPE_DIRECTIONAL)
    {
        float3 light_dir       = light_direction(light);

        L = light_dir;
        attenuation = 1.0f;
    }
    else if (type == LIGHT_TYPE_POINT)
    {        
        float3  to_light       = light_position(light) - P;
        float3  light_dir       = normalize(to_light);
        float light_distance = length(to_light);
        float radius = light_radius(light);

        L = light_dir;

        if (radius < light_distance)
        {
            attenuation = 0.0f;
        }
        else
        {
            attenuation = (1.0f / (light_distance * light_distance));
        }
    }
    else
    {
        float3  to_light       = light_position(light) - P;
        float3  light_dir      = normalize(to_light);
        float light_distance = length(to_light);

        L = light_dir;

        float angle_attenuation = dot(L, light_direction(light));
        angle_attenuation       = smoothstep(light_cos_theta_outer(light), light_cos_theta_inner(light), angle_attenuation);    
        attenuation = (angle_attenuation / (light_distance * light_distance));    
    }

    H  = normalize(Wo + L); 
    attenuation *= clamp(dot(N, L), 0.0, 1.0);
}

float3 direct_lighting(
    in Light light,   
    in float3 V, 
    in float3 N, 
    in float3 P, 
    in float3 F0, 
    in float3 diffuse_color, 
    in float roughness
)
{
    float3 Lo = float3(0.0f, 0.0f, 0.0f);

    float3 T = float3(1.0f, 1.0f, 1.0f);

    float3 ray_origin = P + N * 0.1f;

    static bool phong = false;

    float3 Li;
    float3 L;
    float3 H;
    float attenuation;

    fetch_light_properties(light, 
                            V, 
                            P, 
                            N, 
                            Li, 
                            L, 
                            H, 
                            attenuation);

	if (attenuation > 0)
#if USE_PHONG
    {
        float NdotL = max(0.0, dot(N, L));
        float3 diff = Li * diffuse_color * NdotL;

        // Specular part
        // Specular map values are stored in alpha of albedo mrt
        float spec = pow(max(dot(N, H), 0.0f), 32.0f);

        //Lo += diff + spec;
        Lo += (diff + spec) * attenuation;
    }
#else // USE_PHONG
    {
        float3  brdf      = calculate_brdf(diffuse_color, roughness, N, F0, V, H, L);
        Lo += T * brdf * attenuation * Li;  
    }
#endif

    return Lo;
}
#endif // LIGHT_H