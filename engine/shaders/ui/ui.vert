struct VSInput
{
    [[vk::location(0)]] float2 pos : POSITION;
    [[vk::location(1)]] float2 uv : TEXCOORD0;
    [[vk::location(2)]] float4 color : COLOR0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    [[vk::location(0)]] float2 uv : TEXCOORD0;
    [[vk::location(1)]] float4 color : COLOR0;
};

struct PushBlock
{
    float2 scale;
    float2 translate;
};
[[vk::push_constant]] PushBlock pushConstants;

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    output.uv = input.uv;
    output.color = input.color;
    output.pos = float4(input.pos * pushConstants.scale + pushConstants.translate, 0.0f, 1.0f);

    return output;
}