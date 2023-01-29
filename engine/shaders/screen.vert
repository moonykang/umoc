#version 450

#extension GL_GOOGLE_include_directive : require

layout(set = 1, binding = 0) uniform ubo_mesh
{
    mat4 model;
} ubo_mesh_0;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 0) out vec2 outUV;

void main() {
    gl_Position = vec4(inPosition.xy, 0.0f, 1.0f);
    gl_Position.y = -gl_Position.y;

	outUV = inUV;
    outUV.y = 1.0 - outUV.y;
}