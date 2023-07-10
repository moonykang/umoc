struct VSOutput
{
    [[vk::location(0)]] float3 color : COLOR0;
};

float4 main(VSOutput vsOutput) : SV_TARGET
{
    return float4(vsOutput.color, 1.0f);
}

