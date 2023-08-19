#include "../common.hlsl"

/*
class Vertex
{
  public:
    glm::vec3 position; // 0, 12
    glm::vec3 normal;   // 12, 12
    glm::vec2 uv;
    glm::vec4 color;
    glm::vec4 joint0;
    glm::vec4 weight0;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};
*/
struct VSInput
{
    [[vk::location(0)]] float3 pos : POSITION;
    [[vk::location(1)]] float3 normal : NORMAL;
    [[vk::location(2)]] float2 uv : TEXCOORD;
    [[vk::location(3)]] float4 color : COLOR0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    [[vk::location(0)]] float3 normal : NORMAL;
    [[vk::location(1)]] float3 color : COLOR0;
    [[vk::location(2)]] float3 view : TEXCOORD1;
    [[vk::location(3)]] float3 light : TEXCOORD2;
    [[vk::location(4)]] float4 shadowUv : TEXCOORD3;
};

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneView;
}

[[vk::binding(1, 0)]] cbuffer ubo
{
    SceneLight sceneLight;
}

[[vk::binding(0, 1)]] cbuffer ubo
{
    Model model;
}


static const float4x4 biasMat = float4x4(
	0.5, 0.0, 0.0, 0.5,
	0.0, 0.5, 0.0, 0.5,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0 );

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    output.color = input.color.xyz;

    float4 worldPos = mul(model.transform, float4(input.pos.xyz, 1.0f));
    output.pos = mul(sceneView.view_proj, worldPos);

    output.normal = mul((float3x3)model.transform, input.normal);

    float3 lightDirection = light_direction(sceneLight.lights[0]);
    output.light = normalize(lightDirection);
    output.view = -worldPos.xyz;
    output.shadowUv = mul(biasMat, mul(sceneLight.lightMatrix, worldPos));

    return output;
}