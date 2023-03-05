#pragma once

#include "common/transform.h"
#include "common/util.h"
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
class VertexInput;
class Instance
{
  public:
    Instance();

    virtual ~Instance() = default;

    Result init(platform::Context* context, VertexInput* vertexInput);

    void terminate(platform::Context* context);

    rhi::DescriptorSet* getDescriptorSet()
    {
        return descriptorSet;
    }

    Result updateUniformBuffer(platform::Context* context);

  private:
    struct UniformBufferObject
    {
        glm::mat4 transform;
    } ubo;

    util::Transform transform;

    rhi::UniformBuffer* uniformBuffer;
    rhi::DescriptorSet* descriptorSet;
    VertexInput* vertexInput;

    std::mutex mutex;
    bool initialized;
};
} // namespace model