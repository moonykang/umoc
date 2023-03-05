#pragma once
#include "common/transform.h"
#include "common/util.h"
#include <mutex>

namespace rhi
{
class UniformBuffer;
class DescriptorSet;
} // namespace rhi

namespace platform
{
class Context;
}

namespace scene
{
class View
{
  public:
    View();

    ~View();

    Result init(platform::Context* context);

    void terminate(platform::Context* context);

    rhi::DescriptorSet* getDescriptorSet()
    {
        return descriptorSet;
    }

    void setView(glm::vec3 position, glm::vec3 lookAt, glm::vec3 headUp);

    void setPerspective(float fov, float ratio, float minDepth, float maxDepth);

    Result updateUniformBuffer(platform::Context* context);

  private:
    std::mutex mutex;
    bool dirty;
    struct UniformBufferObject
    {
        glm::mat4 view;
        glm::mat4 perspective;
    } ubo;

    rhi::UniformBuffer* uniformBuffer;
    rhi::DescriptorSet* descriptorSet;

    static constexpr size_t uniformDataSize = sizeof(UniformBufferObject);
};
} // namespace scene