#version 450

layout(set=0, binding = 0) uniform GlobalUniformBufferObject {
    mat4 view;
    mat4 proj;
} gubo;

layout(set = 1, binding = 0) uniform LocalUniformBufferObject {
    mat4 model;
} lubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = gubo.proj * gubo.view * lubo.transform * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}