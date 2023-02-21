#include "quad.h"
#include "rhi/buffer.h"
#include "rhi/context.h"

namespace model
{
Result Quad::load(platform::Context* platformContext, rhi::VertexChannelFlags vertexChannels)
{
    rhi::Context* context = platformContext->getRHI();

    uint32_t indexStart = 0;
    uint32_t vertexStart = 0;

    const uint32_t slices = 2;
    const uint32_t stacks = 2;

    const uint32_t top_left = 0;
    const uint32_t top_right = 1;
    const uint32_t bottom_left = 2;
    const uint32_t bottom_right = 3;

    const uint32_t num_vertices = 4;
    const glm::vec3 positions[num_vertices] = {
        {-1.0f, -1.0f, 0.0f}, {1.0f, -1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}};

    const glm::vec3 normals[num_vertices] = {
        {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}};

    const glm::vec2 uvs[num_vertices] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};

    const glm::vec4 colors[num_vertices] = {
        {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}};

    rhi::DataBuffer<rhi::Vertex> vertexDataBuffer(num_vertices);

    for (uint32_t i = 0; i < num_vertices; i++)
    {
        rhi::Vertex vertex;
        vertex.position = positions[i];
        vertex.uv = uvs[i];
        vertex.color = colors[i];

        vertexDataBuffer.insert(vertex);
    }

    vertexBuffer = reinterpret_cast<rhi::VertexBuffer*>(
        context->getVertexScratchBuffer()->subAllocate(context, vertexDataBuffer.getSize(), vertexDataBuffer.data()));

    return Result::Continue;
}

void Quad::bind(rhi::Context* context)
{
    vertexBuffer->bind(context);
}

} // namespace model