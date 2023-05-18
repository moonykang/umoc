#version 450

layout (location = 0) in vec3 inWorldPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec4 inTangent;

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

layout(set = 2, binding = 0) uniform sampler2D albedoMap;
layout(set = 2, binding = 1) uniform sampler2D normalMap;
layout(set = 2, binding = 2) uniform sampler2D aoMap;
layout(set = 2, binding = 3) uniform sampler2D metallicMap;
layout(set = 2, binding = 4) uniform sampler2D roughnessMap;
layout(set = 2, binding = 5) uniform samplerCube samplerIrradiance;
layout(set = 2, binding = 6) uniform sampler2D samplerBRDFLUT;
layout(set = 2, binding = 7) uniform samplerCube prefilteredMap;

layout (location = 0) out vec4 outColor;

vec3 calculateNormal()
{
	vec3 tangentNormal = texture(normalMap, inUV).xyz * 2.0 - 1.0;

	vec3 N = normalize(inNormal);
	vec3 T = normalize(inTangent.xyz);
	vec3 B = normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);
	return normalize(TBN * tangentNormal);
}

void main()
{
	vec3 N = calculateNormal();

	vec3 V = normalize(sceneView.cam_pos.xyz - inWorldPos);
	vec3 R = reflect(-V, N); 

    vec3 color = texture(albedoMap, inUV).rgb;

	outColor = vec4(color, 1.0);
}