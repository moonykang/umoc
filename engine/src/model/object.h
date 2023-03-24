#pragma once

#include "common/util.h"
#include <vector>

namespace platform
{
class Context;
}

namespace rhi
{
class Context;
class VertexBuffer;
class IndexBuffer;
class Texture;
} // namespace rhi

namespace model
{
class Node;
class Material;
class VertexInput;
class Instance;

class Object
{
  public:
    Object();

    ~Object() = default;

    Result init(platform::Context* context);

    void terminate(platform::Context* context);

    void draw(platform::Context* context);

  public:
    VertexInput* getVertexInput();

    void addTexture(rhi::Texture* texture);

    rhi::Texture* getTexture(uint32_t index);

    void addMaterial(Material* material);

    Material* getMaterial(int32_t index);

    void addNode(Node* node);

    void addLinearNode(Node* node);

  public:
    Instance* instantiate(platform::Context* context, glm::mat4 transform, bool initDescriptor);

    std::vector<Instance*>& getInstances();

  protected:
    VertexInput* vertexInput;
    std::vector<Material*> materials;
    std::vector<rhi::Texture*> textures;
    std::vector<Node*> nodes;
    std::vector<Node*> linearNodes;
    std::vector<Instance*> linearInstances;
};
} // namespace model
