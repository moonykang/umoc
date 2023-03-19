#version 450

layout(set = 0, binding = 0) uniform PerFrameUBO
{
    mat4  view_inverse;
    mat4  proj_inverse;
    mat4  view_proj_inverse;
    mat4  prev_view_proj;
    mat4  view_proj;
    vec4  cam_pos;
} globalUBO;

layout(set = 1, binding = 0) uniform LocalUniformBufferObject {
    mat4 transform;
} lubo;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;

layout (location = 0) out vec3 outPos;
layout (location = 1) out vec2 outUV;
layout (location = 2) out vec3 outNormal;

void main() {
    vec4 world_pos = lubo.transform * vec4(inPosition, 1.0);

    gl_Position = globalUBO.view_proj * world_pos;
    outPos = world_pos.xyz;
    outUV = inUV;

    vec4 temp = globalUBO.view_proj * world_pos;
    mat4 temp2 = globalUBO.view_proj;
    vec4 temp3 = globalUBO.view_inverse * world_pos;
    vec4 temp4 = globalUBO.view_proj_inverse * world_pos;
    vec4 temp5 = globalUBO.prev_view_proj * world_pos;
    vec4 tmep6 = globalUBO.cam_pos;

    mat3 normal_mat = mat3(lubo.transform);
    outNormal    = normal_mat * inNormal;
}
