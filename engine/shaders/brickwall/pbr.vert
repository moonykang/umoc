#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 4) in vec3 inTangent;

layout (location = 0) out vec3 outWorldPos;
layout (location = 1) out vec3 outNormal;

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
} lubo;

void main()
{
    vec4 world_pos = lubo.transform * vec4(inPosition, 1.0);
    outWorldPos = world_pos.xyz;
    outNormal = mat3(lubo.transform) * inNormal;
    gl_Position = sceneView.view_proj * world_pos;
}