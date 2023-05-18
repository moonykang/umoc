#include "cube.h"

#include "model/material.h"
#include "model/node.h"
#include "model/object.h"
#include "model/vertexInput.h"

namespace model
{
namespace predefined
{
Cube::Cube() : Object(4, 6)
{
}

Result Cube::loadVertexBuffer(platform::Context* context)
{
    const uint32_t slices = 2;
    const uint32_t stacks = 2;

    const uint32_t top_left = 0;
    const uint32_t top_right = 1;
    const uint32_t bottom_left = 2;
    const uint32_t bottom_right = 3;

    const uint32_t vertexCount = 4;
    const glm::vec3 positions[vertexCount] = {
        {-1.0f, -1.0f, 0.0f}, {1.0f, -1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}};

    const glm::vec3 normals[vertexCount] = {
        {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}};

    const glm::vec2 uvs[vertexCount] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};

    const glm::vec4 colors[vertexCount] = {
        {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}};

    std::vector<rhi::Vertex> vertices;

    for (uint32_t i = 0; i < vertexCount; i++)
    {
        rhi::Vertex vertex;
        vertex.position = positions[i];
        vertex.uv = uvs[i];
        vertex.color = colors[i];

        vertices.push_back(vertex);
    }

    return vertexInput->loadVertexBuffer(context, vertices);
}

Result Cube::loadIndexBuffer(platform::Context* context)
{
    const uint32_t indexCount = 6;
    std::vector<uint32_t> indices;
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(1);
    indices.push_back(3);

    return vertexInput->loadIndexBuffer(context, indices);
}
} // namespace predefined
} // namespace model