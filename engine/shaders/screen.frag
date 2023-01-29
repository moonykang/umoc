#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 inUV;
layout(location = 0) out vec4 outColor;

void main() {
    vec4 color = texture(texSampler, inUV);
    outColor = vec4(color.xyz, 1.0f);
}
