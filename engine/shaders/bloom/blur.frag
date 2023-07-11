struct VSOutput
{
    [[vk::location(0)]] float2 uv : TEXCOORD;
};

[[vk::binding(1, 0)]] Texture2D screenInputTexture;
[[vk::binding(1, 0)]] SamplerState screenInputSampler;

[[vk::constant_id(0)]] const int blurdirection = 0;

float4 main(VSOutput input) : SV_TARGET
{
    float weight[5];
	weight[0] = 0.227027;
	weight[1] = 0.1945946;
	weight[2] = 0.1216216;
	weight[3] = 0.054054;
	weight[4] = 0.016216;

    float2 textureSize;
	screenInputTexture.GetDimensions(textureSize.x, textureSize.y);

    float2 offset = 1.0f / textureSize;
    float3 result = screenInputTexture.Sample(screenInputSampler, input.uv).xyz * weight[0];

    if (blurdirection == 1)
    {
        for (int i = 1; i < 5; i++)
        {
            result += screenInputTexture.Sample(screenInputSampler, input.uv + float2(offset.x * i, 0.0f)).xyz * weight[i];
            result += screenInputTexture.Sample(screenInputSampler, input.uv - float2(offset.x * i, 0.0f)).xyz * weight[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; i++)
        {
            result += screenInputTexture.Sample(screenInputSampler, input.uv + float2(0.0f, offset.y * i)).xyz * weight[i];
            result += screenInputTexture.Sample(screenInputSampler, input.uv - float2(0.0f, offset.y * i)).xyz * weight[i];
        }
    }

    return float4(result, 1.0f);
}