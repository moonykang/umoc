struct VSOutput
{
    [[vk::location(0)]] float2 uv : TEXCOORD;
};

[[vk::binding(1, 0)]] Texture2D screenInputTexture;
[[vk::binding(1, 0)]] SamplerState screenInputSampler;

float4 main(VSOutput input) : SV_TARGET
{
    float4 color = float4(screenInputTexture.Sample(screenInputSampler, input.uv).xyz, 1.0f);

    float brightness = dot(color.xyz, float3(0.2126, 0.7152, 0.0722));


    if (brightness >= 1.0f)
    {
        return color;
    }
    else
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }
}