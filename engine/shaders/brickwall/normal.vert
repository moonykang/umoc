#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;

layout (location = 0) out vec3 outWorldPos;
layout (location = 1) out vec2 outUV;
layout (location = 2) out vec3 outTangentLightPos;
layout (location = 3) out vec3 outTangentViewPos;
layout (location = 4) out vec3 outTangentFragPos;


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

layout(set = 1, binding = 0) uniform LocalUniformBufferObject {
    mat4 transform;
    mat3 transform_inverse;
} lubo;

void main() 
{
    vec4 world_pos = lubo.transform * vec4(inPosition, 1.0);
    outWorldPos = world_pos.xyz;
    outUV = inUV;

    //mat3 normalMatrix = transpose(inverse(mat3(lubo.transform)));
    /*
    mat3 normalMatrix = transpose(lubo.transform_inverse);
    vec3 T = normalize(normalMatrix * inTangent);
    vec3 N = normalize(normalMatrix * inNormal);
    T = normalize(T - dot(T, N) * N);
    */

    vec3 N = normalize(inNormal);
    vec3 T = normalize(inTangent);
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);


    //mat3 TBN = transpose(mat3(T, B, N));
    outTangentLightPos = TBN * sceneLight.lightPosition.xyz;
    outTangentViewPos = TBN * sceneView.cam_pos.xyz;
    outTangentFragPos = TBN * outWorldPos;

    gl_Position = sceneView.view_proj * world_pos;
}