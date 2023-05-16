#include "object.h"

#include "model/material.h"
#include "model/node.h"
#include "model/object.h"
#include "model/vertexInput.h"

namespace model
{
namespace predefined
{
Object::Object(uint32_t vertexCount, uint32_t indexCount) : vertexCount(vertexCount), indexCount(indexCount)
{
}

Result Object::loadMesh()
{
    auto localMaterial = getMaterial(-1);

    Node* newNode = new Node(nullptr);
    Mesh* newMesh = new Mesh("noname");
    newNode->setMesh(newMesh);

    Primitive* newPrimitive = new Primitive(0, indexCount, localMaterial);
    newPrimitive->firstVertex = 0;
    newPrimitive->vertexCount = vertexCount;

    newMesh->addPrimitive(newPrimitive);
    addNode(newNode);
    addLinearNode(newNode);

    return Result::Continue;
}
} // namespace predefined
} // namespace model