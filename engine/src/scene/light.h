#pragma once

#include "common/util.h"
#include <mutex>

namespace rhi
{
class UniformBuffer;
} // namespace rhi

namespace platform
{
class Context;
}

namespace scene
{
class Light
{
  public:
    Light();

    virtual ~Light() = default;

    Result init(platform::Context* context);

    void terminate(platform::Context* context);

    void setLight(glm::vec4 position);

    Result updateUniformBuffer(platform::Context* context);

    rhi::UniformBuffer* getUniformBuffer();

  private:
    std::mutex mutex;
    bool dirty;

    struct UniformBufferObject
    {
        ALIGNED(4)
        glm::vec4 position;
        float gamma = 0.1f;
        float exposure = 0.1f;

    } ubo;

    rhi::UniformBuffer* uniformBuffer;

    static constexpr size_t uniformDataSize = sizeof(UniformBufferObject);

    // input
    glm::vec3 position;
};
} // namespace scene