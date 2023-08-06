#pragma once

#include "common/util.h"
#include "defines.h"
#include <vector>

namespace platform
{
class Context;
}

namespace rhi
{
class Context;
class SubAllocatedBuffer;
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

    virtual ~Object() = default;

    Result init(platform::Context* context);

    void terminate(platform::Context* context);

    void draw(platform::Context* context);

  public:
    VertexInput* getVertexInput();

    void addTexture(TextureID id);

    TextureID getTexture(uint32_t index);

    void addMaterial(Material* material);

    Material* getMaterial(int32_t index);

    void addNode(Node* node);

    void addLinearNode(Node* node);

  public:
    Instance* instantiate(platform::Context* context, glm::mat4 transform, const bool initDescriptor,
                          const bool ignoreHierarchy = false);

    std::vector<Instance*>& getInstances();

  protected:
    VertexInput* vertexInput;
    std::vector<Material*> materials;
    std::vector<Node*> nodes;
    std::vector<Node*> linearNodes;
    std::vector<Instance*> linearInstances;

    std::vector<TextureID> textures;
};
} // namespace model
