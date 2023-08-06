struct VSOutput
{
    float4 pos : SV_POSITION;
    [[vk::location(0)]] float2 uv : TEXCOORD0;
    [[vk::location(1)]] float4 color : COLOR0;
};

[[vk::binding(0, 0)]] Texture2D fontTexture;
[[vk::binding(0, 0)]] SamplerState fontSampler;


float4 main(VSOutput input) : SV_TARGET
{
    float4 fontColor = fontTexture.Sample(fontSampler, input.uv);
    return input.color * fontColor;
}