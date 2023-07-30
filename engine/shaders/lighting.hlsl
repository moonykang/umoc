#ifndef LIGHTING_HLSL
#define LIGHTING_HLSL

#include "brdf.hlsl"
// ------------------------------------------------------------------------

void fetch_light_properties(
    in Light light,
    in float3 Wo,
    in float3 P,
    in float3 N,
    out float3 Li,
    out float3 Wi,
    out float3 Wh,
    out float attenuation
)
{
    const int type = light_type(light);
    Li = light_color(light) * light_intensity(light);  

    if (type == LIGHT_TYPE_DIRECTIONAL)
    {
        float3 light_dir       = light_direction(light);
        Wi = light_dir;
        attenuation = 1.0f;
    }
    else if (type == LIGHT_TYPE_POINT)
    {        
        float3  to_light       = light_position(light) - P;
        float3  light_dir       = normalize(to_light);
        float light_distance = length(to_light);
        Wi = light_dir;
        attenuation = (1.0f / (light_distance * light_distance));
    }
    else // spot light
    {
        float3  to_light       = light_position(light) - P;
        float3  light_dir      = normalize(to_light);
        float light_distance = length(to_light);
        Wi = light_dir;
        
        float angle_attenuation = dot(Wi, light_direction(light));
        angle_attenuation       = smoothstep(light_cos_theta_outer(light), light_cos_theta_inner(light), angle_attenuation);    
        attenuation = (angle_attenuation / (light_distance * light_distance));    
    }

    Wh  = normalize(Wo + Wi);
    attenuation *= clamp(dot(N, Wi), 0.0, 1.0);
}

float3 direct_lighting(
    in Light light,   
    in float3 Wo, 
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

    // Punctual Light
    {
        float3 Li;
        float3 Wi;
        float3 Wh;
        float attenuation;

        fetch_light_properties(light, 
                               Wo, 
                               P, 
                               N, 
                               Li, 
                               Wi, 
                               Wh, 
                               attenuation);

        float3 brdf = reflection_brdf(diffuse_color, roughness, N, F0, Wo, Wh, Wi);
        Lo += T * brdf * attenuation * Li;  
        //Lo += T * brdf * Li;
    }

    return Lo;
}
#endif // LIGHTING_HLSL