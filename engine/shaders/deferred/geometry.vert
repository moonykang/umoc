struct VSInput
{
    [[vk::location(0)]] float3 pos : POSITION0;
    [[vk::location(1)]] float2 uv : TEXCOORD0;
    [[vk::location(2)]] float3 color : COLOR0;
    [[vk::location(3)]] float3 normal : NORMAL0;
    [[vk::location(4)]] float3 tangent : TEXCOORD1;
};

struct VSOutput
{
	float4 pos : SV_POSITION;
    [[vk::location(0)]] float3 normal : NORMAL0;
    [[vk::location(1)]] float2 uv : TEXCOORD0;
    [[vk::location(2)]] float3 color : COLOR0;
    [[vk::location(3)]] float3 worldPos : POSITION0;
    [[vk::location(4)]] float3 tangent : TEXCOORD1;
};

struct SceneUBO
{
    float4x4 view_inverse;
    float4x4 proj_inverse;
    float4x4 view_proj_inverse;
    float4x4 prev_view_proj;
    float4x4 view_proj;
    float4 view_pos;
};

[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneUBO sceneUBO;
}

struct LightUBO
{
    float4 pos;
    float gamma;
    float exposure;
};

[[vk::binding(1, 0)]] cbuffer ubo
{
    LightUBO lightUBO;
}

struct ModelUBO
{
    float4x4 transform;
    float3x3 transform_inverse;
};

[[vk::binding(0, 1)]] cbuffer ubo
{
    ModelUBO modelUBO;
}

VSOutput main(VSInput input)
{
	VSOutput output = (VSOutput)0;

    float4 worldPos = mul(modelUBO.transform, float4(input.pos, 1.0f));
    output.worldPos = worldPos.xyz;

	output.pos = mul(sceneUBO.view_proj, worldPos);

	output.uv = input.uv;

	// Normal in world space
	output.normal = normalize(input.normal);
	output.tangent = normalize(input.tangent);

	// Currently just vertex color
	output.color = input.color;
	return output;
}