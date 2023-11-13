#include "cube.h"

#include "model/material.h"
#include "model/node.h"
#include "model/object.h"
#include "model/vertexInput.h"

namespace model
{
namespace predefined
{

Cube::Cube() : Object(36, 0)
{
}

Result Cube::loadVertexBuffer(platform::Context* context, float uvScale)
{
    const uint32_t vertexCount = 36;

    const glm::vec3 positions[vertexCount] = {
        // back face
        {-1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f, 1.0f, -1.0f},
        {1.0f, 1.0f, -1.0f},
        {-1.0f, 1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
        // front face
        {-1.0f, -1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        {1.0f, -1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        {-1.0f, -1.0f, 1.0f},
        {-1.0f, 1.0f, 1.0f},
        // left face
        {-1.0f, 1.0f, 1.0f},
        {-1.0f, -1.0f, -1.0f},
        {-1.0f, 1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
        {-1.0f, 1.0f, 1.0f},
        {-1.0f, -1.0f, 1.0f},
        // right face
        {1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        // bottom face
        {-1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, 1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, 1.0f},
        {-1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f, 1.0f},
        // top face
        {-1.0f, 1.0f, -1.0f},
        {1.0f, 1.0f, -1.0f},
        {1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        {-1.0f, 1.0f, 1.0f},
        {-1.0f, 1.0f, -1.0f},
    };

    const glm::vec3 normals[vertexCount] = {
        // back face
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        // front face
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        // left face
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        // right face
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        // bottom face
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        // top face
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
    };

    const glm::vec2 uvs[vertexCount] = {
        // back face
        {0.0f, 0.0f}, // bottom-left
        {1.0f, 0.0f}, // bottom-right
        {1.0f, 1.0f}, // top-right
        {1.0f, 1.0f}, // top-right
        {0.0f, 1.0f}, // top-left
        {0.0f, 0.0f}, // bottom-left
        // front face
        {0.0f, 0.0f}, // bottom-left
        {1.0f, 1.0f}, // top-right
        {1.0f, 0.0f}, // bottom-right
        {1.0f, 1.0f}, // top-right
        {0.0f, 0.0f}, // bottom-left
        {0.0f, 1.0f}, // top-left
        // left face
        {1.0f, 0.0f}, // top-right
        {0.0f, 1.0f}, // bottom-left
        {1.0f, 1.0f}, // top-left
        {0.0f, 1.0f}, // bottom-left
        {1.0f, 0.0f}, // top-right
        {0.0f, 0.0f}, // bottom-right
                      // right face
        {1.0f, 0.0f}, // top-left
        {1.0f, 1.0f}, // top-right
        {0.0f, 1.0f}, // bottom-right
        {0.0f, 1.0f}, // bottom-right
        {0.0f, 0.0f}, // bottom-left
        {1.0f, 0.0f}, // top-left
        // bottom face
        {0.0f, 1.0f}, // top-right
        {1.0f, 0.0f}, // bottom-left
        {1.0f, 1.0f}, // top-left
        {1.0f, 0.0f}, // bottom-left
        {0.0f, 1.0f}, // top-right
        {0.0f, 0.0f}, // bottom-right
        // top face
        {0.0f, 1.0f}, // top-left
        {1.0f, 1.0f}, // top-right
        {1.0f, 0.0f}, // bottom-right
        {1.0f, 0.0f}, // bottom-right
        {0.0f, 0.0f}, // bottom-left
        {0.0f, 1.0f}, // top-left
    };

    const glm::vec4 colors[vertexCount] = {
        {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f},

        {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f},

        {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f},

        {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f},

        {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f},

        {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f},
    };

    std::vector<rhi::Vertex> vertices;

    for (uint32_t i = 0; i < vertexCount; i++)
    {
        rhi::Vertex vertex;

        vertex.position = positions[i];
        vertex.uv = uvs[i] * uvScale;
        vertex.color = colors[i];
        vertex.normal = normals[i];
        vertex.tangent = {0.0f, 0.0f, 0.0f};

        vertices.push_back(vertex);
    }

    return vertexInput->loadVertexBuffer(context, vertices);
}

Result Cube::loadIndexBuffer(platform::Context* context)
{
    return Result::Continue;
}
} // namespace predefined
} // namespace model