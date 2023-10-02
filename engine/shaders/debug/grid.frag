#include "../common.hlsl"

struct VSOutput
{
    float4 pos : SV_POSITION;
    [[vk::location(0)]] float3 nearPoint : POSITION0;
    [[vk::location(1)]] float3 farPoint : POSITION1;
};

struct FSOutput
{
    float4 color : SV_TARGET;
    float depth : SV_Depth;
};


[[vk::binding(0, 0)]] cbuffer ubo
{
    SceneView sceneView;
}

float4 grid(float3 worldPos, float scale)
{
    float2 coord = worldPos.xz * scale; // use the scale variable to set the distance between the lines
    float2 derivative = fwidth(coord);
    float2 grid = abs(frac(coord - 0.5) - 0.5) / derivative;
    float gridLine = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);

    float4 color = float4(0.2, 0.2, 0.2, 1.0 - min(gridLine, 1.0));

    // z axis
    if(worldPos.x > -0.1 * minimumx && worldPos.x < 0.1 * minimumx)
        color.z = 1.0;
    // x axis
    if(worldPos.z > -0.1 * minimumz && worldPos.z < 0.1 * minimumz)
        color.x = 1.0;

    return color;
}

float computeDepth(float3 pos)
{
    float4 clip_space_pos = mul(sceneView.view_proj, float4(pos.xyz, 1.0));
    return (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(float3 pos)
{
    float4 clip_space_pos = mul(sceneView.view_proj, float4(pos.xyz, 1.0));
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * sceneView.nearPlane * sceneView.farPlane) / (sceneView.farPlane + sceneView.nearPlane - clip_space_depth * (sceneView.farPlane - sceneView.nearPlane)); // get linear value between 0.01 and 100
    return linearDepth / sceneView.farPlane; // normalize
}

FSOutput main(VSOutput input)
{
    FSOutput output = (FSOutput) 0;

    float t = -input.nearPoint.y / (input.farPoint.y - input.nearPoint.y);
    float3 worldPos = input.nearPoint + t * (input.farPoint - input.nearPoint);

    output.depth = computeDepth(worldPos);

    float linearDepth = computeLinearDepth(worldPos);
    float fading = max(0.f, (0.5f - linearDepth));

    output.color = (grid(worldPos, 10) + grid(worldPos, 1)) * float(t > 0);
    output.color.a *= fading;

    return output;
}