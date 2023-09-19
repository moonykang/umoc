struct VSOutput
{
    [[vk::location(0)]] float2 uv : TEXCOORD;
};

[[vk::binding(0, 0)]] Texture2D screenTexture;
[[vk::binding(0, 0)]] SamplerState screenSampler;

float4 main(VSOutput output) : SV_TARGET
{
    float3 color = screenTexture.Sample(screenSampler, output.uv).xyz;

    return float4(color, 1.0f);
}