#define NUM_LIGHTS 6

struct Light
{
    float4 pos;
    float3 color;
    float radius;
};

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
    float4x4 proj;
    float4 pos;
	float nearPlane;
	float farPlane;
};