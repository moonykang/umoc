struct VSOutput
{
    [[vk::location(0)]] float3 worldPos : POSITION0;
    [[vk::location(1)]] float2 uv : TEXCOORD;
    [[vk::location(2)]] float3 tangentLightPos : POSITION1;
    [[vk::location(3)]] float3 tangentViewPos : POSITION2;
    [[vk::location(4)]] float3 tangentFragPos : POSITION3;
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

[[vk::binding(0, 2)]] Texture2D albedoMapTexture;
[[vk::binding(0, 2)]] SamplerState albedoMapSampler;
[[vk::binding(1, 2)]] Texture2D normalMapTexture;
[[vk::binding(1, 2)]] SamplerState normalMapSampler;

float4 main(VSOutput vsOutput) : SV_TARGET
{
    float3 normal = normalMapTexture.Sample(normalMapSampler, vsOutput.uv).xyz;
    normal = normalize(normal * 2.0f - 1.0f);

    float3 albedo = albedoMapTexture.Sample(albedoMapSampler, vsOutput.uv).xyz;
    float3 ambient = albedo * 0.2f;

    // diffuse
    float3 L = normalize(vsOutput.tangentLightPos - vsOutput.tangentFragPos);
    float diff = max(dot(L, normal), 0.0f);
    float3 diffuse = diff * albedo;

    // specular
    float3 V = normalize(vsOutput.tangentViewPos - vsOutput.tangentFragPos);
    float3 R = reflect(-L, normal);
    float3 H = normalize(L + V);
    float SP = pow(max(dot(normal, H), 0.0f), 32.0f);

    float3 specular = mul(float3(0.2f, 0.2f, 0.2f), SP);

    return float4(ambient + diffuse + specular, 1.0f);
}