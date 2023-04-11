#include "loader.h"
#include "model/material.h"
#include "model/node.h"
#include "model/object.h"
#include "model/vertexInput.h"

namespace model
{
namespace predefined
{
Loader::Builder::Builder() : material(nullptr)
{
}

Loader::Builder& Loader::Builder::setMaterial(Material* material)
{
    this->material = material;
    return *this;
}

Loader::Builder& Loader::Builder::setShaderParameters(rhi::ShaderParameters* shaderParameters)
{
    this->shaderParameters = shaderParameters;
    return *this;
}

std::shared_ptr<Loader> Loader::Builder::build()
{
    return std::make_shared<Loader>(material, shaderParameters);
}

Loader::Loader(Material* material, rhi::ShaderParameters* shaderParameters)
    : material(material), shaderParameters(shaderParameters)
{
}

Object* Loader::load(platform::Context* context, scene::SceneInfo* sceneInfo)
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

    if (material)
    {
        try_call(material->init(context));
        try_call(material->update(context));
        newObject->addMaterial(material);
    }
    else
    {
        try_call(loadMaterial(context, newObject));
    }

    auto localMaterial = newObject->getMaterial(-1);

    if (shaderParameters)
    {
        localMaterial->setShaderParameters(shaderParameters);
    }

    Node* newNode = new Node(nullptr);
    Mesh* newMesh = new Mesh("noname");
    newNode->setMesh(newMesh);

    Primitive* newPrimitive = new Primitive(indexStart, indexCount, localMaterial);
    newPrimitive->firstVertex = vertexStart;
    newPrimitive->vertexCount = vertexCount;

    newMesh->addPrimitive(newPrimitive);
    newObject->addNode(newNode);
    newObject->addLinearNode(newNode);

    return newObject;
}
} // namespace predefined
} // namespace model