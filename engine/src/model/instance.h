#pragma once

#include "common/transform.h"
#include "common/tree.h"
#include "common/util.h"
#include <memory>
#include <mutex>
#include <vector>

namespace rhi
{
class Descriptor;
class DescriptorSet;
class UniformBuffer;
} // namespace rhi

namespace platform
{
class Context;
}

namespace model
{
class Object;
class Material;

class Instance
{
  public:
    Instance(Object* object, Material* material, uint32_t firstIndex, uint32_t indexCount, uint32_t firstVertex,
             uint32_t vertexCount, glm::mat4 transform);

    virtual ~Instance() = default;

    Result init(platform::Context* context, bool initDescriptor);

    void terminate(platform::Context* context);

    rhi::DescriptorSet* getDescriptorSet()
    {
        return descriptorSet;
    }

    Result updateUniformBuffer(platform::Context* context);

    void draw(platform::Context* context);

    Material* getMaterial();

  private:
    Object* object;
    Material* material;

    uint32_t firstIndex;
    uint32_t indexCount;
    uint32_t firstVertex;
    uint32_t vertexCount;

    struct UniformBufferObject
    {
        glm::mat4 transform;
    } ubo;

    util::Transform transform;

    rhi::UniformBuffer* uniformBuffer;
    rhi::DescriptorSet* descriptorSet;

    std::mutex mutex;
    bool initialized;
};
} // namespace model
