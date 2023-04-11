#pragma once
#include "common/tree.h"
#include "common/util.h"

namespace model
{
class Material;
class Primitive
{
  public:
    uint32_t firstIndex;
    uint32_t indexCount;
    uint32_t firstVertex;
    uint32_t vertexCount;
    Material* material;

    Primitive(uint32_t firstIndex, uint32_t indexCount, Material* material)
        : firstIndex(firstIndex), indexCount(indexCount), material(material){};
};

class Mesh
{
  public:
    Mesh(std::string name);

    void addPrimitive(Primitive* primitive);

    std::vector<Primitive*>& getPrimitives();

  private:
    std::vector<Primitive*> primitives;
    std::string name;
};

class Node : public util::Tree
{
  public:
    Node(Node* parent);

    ~Node() = default;

    void terminate();

    void updateTransform();

    glm::mat4 getTransform();

    bool hasMesh();

    std::vector<Primitive*>& getPrimitives();

    void setMesh(Mesh* mesh);

  public:
    uint32_t index;
    std::vector<Node*> children;
    glm::mat4 matrix;
    std::string name;
    Mesh* mesh;
    int32_t skinIndex;
    glm::vec3 translation;
    glm::vec3 scale;
    glm::quat rotation;
};
} // namespace model