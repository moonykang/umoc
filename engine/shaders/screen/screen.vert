struct VSInput
{
    uint id : SV_VertexID;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    [[vk::location(0)]] float2 uv : TEXCOORD;
};

VSOutput main(VSInput vsInput)
{
    VSOutput output = (VSOutput) 0;
    output.uv = float2((vsInput.id << 1) & 2, vsInput.id & 2);
    output.pos = float4(mul(output.uv, 2.0f) - 1.0f, 0.0f, 1.0f);
    return output;
}