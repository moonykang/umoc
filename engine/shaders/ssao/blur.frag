
struct VSOutput
{
    [[vk::location(0)]] float2 uv : TEXCOORD;
};

[[vk::binding(1, 0)]] Texture2D inputTexture;
[[vk::binding(1, 0)]] SamplerState inputSampler;

float4 main(VSOutput input) : SV_TARGET
{
    int2 texDim;
	inputTexture.GetDimensions(texDim.x, texDim.y);

    float2 texelSize = 1.0f / texDim;

    float result = 0.0f;

    for (int x = -2; x < 2; x++)
    {
        for (int y = -2; y < 2; y++)
        {
            float2 offset = mul(float2((float)x, (float)y), texelSize);
            result += inputTexture.Sample(inputSampler, input.uv + offset).r;
        }
    }
    result = result / (4.0f * 4.0f);
    return float4(result, result, result, 1.0f);
}