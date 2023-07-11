struct VSOutput
{
    [[vk::location(0)]] float3 uvw : TEXCOORD0;
};


[[vk::binding(0, 2)]] TextureCube environMapTexture;
[[vk::binding(0, 2)]] SamplerState environMapSampler;

float4 main(VSOutput input) : SV_TARGET
{
    float3 color = environMapTexture.Sample(environMapSampler, input.uvw).rgb;
    return float4(color, 1.0f);
}