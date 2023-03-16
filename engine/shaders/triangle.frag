#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

layout(location = 0) out vec4 outColor;

layout(set = 2, binding = 0) uniform ubo_material
{
    vec4 materialFactors; // x: alphaCutoff, y: metallicFactor, z: roughnessFactor, w: reserved
	vec4 baseColorFactor;
} materialUBO;

layout (set = 2, binding = 1) uniform sampler2D textureAlbedo;

void main() {
    vec4 albedo = texture(textureAlbedo, inUV);

    outColor = vec4(albedo.xyz, 1.0);
}