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

    // rhi::DataBuffer<rhi::Vertex> vertexDataBuffer(num_vertices);
    std::vector<rhi::Vertex> vertexDataBuffer;

    for (uint32_t i = 0; i < num_vertices; i++)
    {
        rhi::Vertex vertex;
        vertex.position = positions[i];
        vertex.uv = uvs[i];
        vertex.color = colors[i];

        vertexDataBuffer.push_back(vertex);

        // vertexDataBuffer.insert(std::move(vertex));
    }

    vertexBuffer = reinterpret_cast<rhi::VertexBuffer*>(
        context->getVertexScratchBuffer()->subAllocate(context, vertexDataBuffer.size(), vertexDataBuffer.data()));

    const uint32_t num_indices = 6;

    // rhi::DataBuffer<uint32_t> indexDataBuffer(num_indices);
    std::vector<uint32_t> indexDataBuffer;
    /*
    indexDataBuffer.insert(0);
    indexDataBuffer.insert(1);
    indexDataBuffer.insert(2);
    indexDataBuffer.insert(2);
    indexDataBuffer.insert(1);
    indexDataBuffer.insert(3);
    */
    indexDataBuffer.push_back(0);
    indexDataBuffer.push_back(1);
    indexDataBuffer.push_back(2);
    indexDataBuffer.push_back(2);
    indexDataBuffer.push_back(1);
    indexDataBuffer.push_back(3);

    indexBuffer = reinterpret_cast<rhi::IndexBuffer*>(
        context->getIndexScratchBuffer()->subAllocate(context, indexDataBuffer.size(), indexDataBuffer.data()));

    return Result::Continue;
}

void Quad::draw(rhi::Context* context)
{
    vertexBuffer->bind(context);
    indexBuffer->bind(context);

    context->drawIndexed(6, 1, 0, 0, 0);
}

} // namespace model