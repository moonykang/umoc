#include "quad.h"
#include "rhi/buffer.h"

namespace model
{
Result Quad::load(platform::Context* context, rhi::VertexChannelFlags vertexChannels)
{
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

    const glm::vec3 colors[num_vertices] = {
        {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}};

    rhi::DataBuffer<rhi::Vertex> vertexBuffer(num_vertices);

    for (uint32_t i = 0; i < num_vertices; i++)
    {
        rhi::Vertex vertex;
        vertex.position = positions[i];
        vertex.uv = uvs[i];

        vertexBuffer.insert(vertex);
    }

    return Result::Continue;
}
} // namespace model