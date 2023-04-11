#include "node.h"

namespace model
{

Mesh::Mesh(std::string name) : name(name)
{
}

void Mesh::addPrimitive(Primitive* primitive)
{
    primitives.push_back(primitive);
}

std::vector<Primitive*>& Mesh::getPrimitives()
{
    return primitives;
}

Node::Node(Node* parent)
    : Tree(parent), index(0), matrix(glm::mat4(1.f)), mesh(nullptr), skinIndex(-1), translation(),
      scale(glm::vec3(1.f)), rotation()
{
}

void Node::terminate()
{
    if (mesh)
    {
        delete mesh;
        mesh = nullptr;
    }

    for (auto& child : children)
    {
        TERMINATE(child);
    }
}

void Node::updateTransform()
{
    glm::mat4 localMatrix = glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) *
                            glm::scale(glm::mat4(1.0f), scale) * matrix;

    matrix = parent == nullptr ? localMatrix : reinterpret_cast<Node*>(parent)->getTransform() * localMatrix;
}

glm::mat4 Node::getTransform()
{
    return matrix;
}

bool Node::hasMesh()
{
    return mesh != nullptr;
}

void Node::setMesh(Mesh* mesh)
{
    ASSERT(!this->mesh);
    this->mesh = mesh;
}

std::vector<Primitive*>& Node::getPrimitives()
{
    return mesh->getPrimitives();
}
} // namespace model