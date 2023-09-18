struct VSOutput
{
    float4 pos : SV_POSITION;
    [[vk::location(0)]] float near : POSITION0;
    [[vk::location(1)]] float far : POSITION1;
    [[vk::location(2)]] float3 nearPoint : POSITION2;
    [[vk::location(3)]] float3 farPoint : POSITION3;
    [[vk::location(4)]] float4x4 fragView : POSITION4;
    [[vk::location(5)]] float4x4 fragProj : POSITION5;
};

struct FSOutput
{
    float4 color : SV_TARGET;
    float depth : SV_Depth;
};

float4 grid(float3 fragPos3D, float scale) {
    float2 coord = fragPos3D.xz * scale; // use the scale variable to set the distance between the lines
    float2 derivative = fwidth(coord);
    float2 grid = abs(frac(coord - 0.5) - 0.5) / derivative;
    float gridLine = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);

    float4 color = float4(0.2, 0.2, 0.2, 1.0 - min(gridLine, 1.0));

    // z axis
    if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
        color.z = 1.0;
    // x axis
    if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
        color.x = 1.0;

    return color;
}

float computeDepth(float3 pos, float4x4 fragView, float4x4 fragProj) {
    float4 clip_space_pos = mul(fragProj, mul(fragView, float4(pos.xyz, 1.0)));
    return (clip_space_pos.z / clip_space_pos.w);
}

FSOutput main(VSOutput input)
{
    FSOutput output = (FSOutput) 0;

    float t = -input.nearPoint.y / (input.farPoint.y - input.nearPoint.y);
    float3 fragPos3D = input.nearPoint + t * (input.farPoint - input.nearPoint);
    float4 gridColor = grid(fragPos3D, 10);

    output.depth = computeDepth(fragPos3D, input.fragView, input.fragProj);

    float4 outColor = gridColor * float(t > 0);
    output.color = outColor;

    return output;
}