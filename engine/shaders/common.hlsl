#ifndef COMMON_H
#define COMMON_H

#define PI 3.14159265359
#define EPSILON 0.0001f
#define INFINITY 100000.0f
#define NUM_LIGHTS 6

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

struct Light
{
    float4 data0;
    float4 data1;
    float4 data2;
    float4 data3;
};

// ------------------------------------------------------------------------

float3 light_direction(in Light light)
{
    return light.data0.xyz;
}

// ------------------------------------------------------------------------

float3 light_color(in Light light)
{
    return light.data2.xyz;
}

// ------------------------------------------------------------------------

float light_intensity(in Light light)
{
    return light.data0.w;
}

// ------------------------------------------------------------------------

float light_radius(in Light light)
{
    return light.data1.w;
}

// ------------------------------------------------------------------------

float3 light_position(in Light light)
{
    return light.data1.xyz;
}

// ------------------------------------------------------------------------

int light_type(in Light light)
{
    return int(light.data3.x);
}

// ------------------------------------------------------------------------

float light_cos_theta_outer(in Light light)
{
    return light.data3.y;
}

// ------------------------------------------------------------------------

float light_cos_theta_inner(in Light light)
{
    return light.data3.z;
}

// ------------------------------------------------------------------------

float3 world_position_from_depth(float2 uv, float depth, float4x4 view_proj_inverse)
{
    // Take texture coordinate and remap to [-1.0, 1.0] range.
    float2 fragPos = uv * 2.0 - 1.0;

    // // Create NDC position.
    float4 ndc_pos = float4(fragPos, depth, 1.0);

    // Transform back into world position.
    float4 worldPos = mul(view_proj_inverse, ndc_pos);

    // Undo projection.
    worldPos = worldPos / worldPos.w;

    return worldPos.xyz;
}
// ------------------------------------------------------------------------

struct SceneLight
{
    Light lights[NUM_LIGHTS];
    uint numLights;
};

struct SceneView
{
    float4x4 view_inverse;
    float4x4 proj_inverse;
    float4x4 view_proj_inverse;
    float4x4 prev_view_proj;
    float4x4 view_proj;
    float4x4 view;
    float4x4 projection;
    float4 pos;
	float nearPlane;
	float farPlane;
};

struct Model
{
    float4x4 transform;
    float3x3 normalMatrix;
};

float4x4 inverse(float4x4 m) {
    float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
    float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
    float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
    float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

    float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
    float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
    float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
    float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
    float idet = 1.0f / det;

    float4x4 ret;

    ret[0][0] = t11 * idet;
    ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
    ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
    ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

    ret[1][0] = t12 * idet;
    ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
    ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
    ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

    ret[2][0] = t13 * idet;
    ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
    ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
    ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

    ret[3][0] = t14 * idet;
    ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
    ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
    ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

    return ret;
}

float3x3 inverse(float3x3 m) {
    float det = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
             m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
             m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

    float inv = 1 / det;

    float3x3 ret;
    ret[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * inv;
    ret[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * inv;
    ret[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * inv;
    ret[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * inv;
    ret[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * inv;
    ret[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * inv;
    ret[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * inv;
    ret[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * inv;
    ret[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * inv;

    return ret;
}

float mix(float x, float y, float a)
{
    return x * (1-a) + y * a;
}  

float2 mix(float2 x, float2 y, float a)
{
    return x * (1-a) + y * a;
} 

float3 mix(float3 x, float3 y, float a)
{
    return x * (1-a) + y * a;
}

float4 mix(float4 x, float4 y, float a)
{
    return x * (1-a) + y * a;
}

#endif // COMMON_H