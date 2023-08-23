struct VSOutput
{
    [[vk::location(0)]] float2 uv : TEXCOORD;
};

[[vk::binding(1, 0)]] Texture2D screenInputTexture;
[[vk::binding(1, 0)]] SamplerState screenInputSampler;

float4 main(VSOutput input) : SV_TARGET
{
    float2 textureSize;
	screenInputTexture.GetDimensions(textureSize.x, textureSize.y);

    float2 srcTexelSize = 1.0f / textureSize;
}