#version 450

layout (location = 0) in vec3 inWorldPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inTangentLightPos;
layout (location = 3) in vec3 inTangentViewPos;
layout (location = 4) in vec3 inTangentFragPos;

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

void main()
{
	vec3 normal = texture(normalMap, inUV).rgb;
	normal = normalize(normal * 2.0f - 1.0f);

    vec3 albedo = texture(albedoMap, inUV).rgb;
    
	vec3 ambient = albedo * 0.2f;

	// diffuse
	vec3 lightDir = normalize(inTangentLightPos - inTangentFragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * albedo;

	// specular
	vec3 viewDir = normalize(inTangentViewPos - inTangentFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	vec3 specular = vec3(0.2) * spec;
	outColor = vec4(ambient + diffuse + specular, 1.0);
}