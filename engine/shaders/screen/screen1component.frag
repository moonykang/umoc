/*
#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 inUV;
layout(location = 0) out vec4 outColor;

void main() {
    vec4 color = texture(texSampler, inUV);
    outColor = vec4(color.xyz, 1.0f);
}
*/

struct VSOutput
{
    [[vk::location(0)]] float2 uv : TEXCOORD;
};

[[vk::binding(1, 0)]] Texture2D screenTexture;
[[vk::binding(1, 0)]] SamplerState screenSampler;

float4 main(VSOutput output) : SV_TARGET
{
    float3 color = screenTexture.Sample(screenSampler, output.uv).xxx;

    return float4(color, 1.0f);
}