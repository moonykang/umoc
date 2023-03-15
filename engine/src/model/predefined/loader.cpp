#include "loader.h"
#include "model/node.h"
#include "model/object.h"
#include "model/vertexInput.h"

namespace model
{
namespace predefined
{
Object* Loader::load(platform::Context* context)
{
    Object* newObject = new Object();
    try_call(newObject->init(context));

    uint32_t indexStart = 0;
    uint32_t vertexStart = 0;

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

    const uint32_t indexCount = 6;
    std::vector<uint32_t> indices;
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(1);
    indices.push_back(3);

    try_call(newObject->getVertexInput()->loadVertexBuffer(context, vertices));
    try_call(newObject->getVertexInput()->loadIndexBuffer(context, indices));

    try_call(loadMaterial(context, newObject));

    Node* newNode = new Node(nullptr);
    Mesh* newMesh = new Mesh("noname");
    newNode->setMesh(newMesh);

    Primitive* newPrimitive = new Primitive(indexStart, indexCount, newObject->getMaterial(-1));
    newPrimitive->firstVertex = vertexStart;
    newPrimitive->vertexCount = vertexCount;

    newMesh->addPrimitive(newPrimitive);
    newObject->addNode(newNode);
    newObject->addLinearNode(newNode);

    return newObject;
}
} // namespace predefined
} // namespace model