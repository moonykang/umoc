struct VSOutput
{
    [[vk::location(0)]] float2 uv : TEXCOORD;
};

[[vk::binding(1, 0)]] Texture2D sceneColorTexture;
[[vk::binding(1, 0)]] SamplerState sceneColorSampler;

[[vk::binding(2, 0)]] Texture2D bloomTexture;
[[vk::binding(2, 0)]] SamplerState bloomSampler;

float4 main(VSOutput input) : SV_TARGET
{
    const float gamma = 1.0 / 2.2f;
    const float exposure = 0.5f;

    float3 sceneColor = sceneColorTexture.Sample(sceneColorSampler, input.uv).xyz;
    float3 bloomColor = bloomTexture.Sample(bloomSampler, input.uv).xyz;

    sceneColor += bloomColor;

    float3 result = float3(1.0f, 1.0f, 1.0f) - exp(-sceneColor * exposure);

    result = pow(result, float3(gamma, gamma, gamma));

    return float4(result.xyz, 1.0f);
}