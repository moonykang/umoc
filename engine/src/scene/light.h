#pragma once

#include "common/util.h"
#include <mutex>

#define NUM_LIGHTS 6

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

    glm::vec4 position;
    glm::vec3 color;
    float radius;
};

class Lights
{
  public:
    Lights();

    virtual ~Lights() = default;

    Result init(platform::Context* context);

    void terminate(platform::Context* context);

    void setLightPosition(uint32_t index, const glm::vec4& position);

    glm::vec4& getLightPosition(uint32_t index);

    void setLightColor(uint32_t index, const glm::vec3& color);

    glm::vec3& getLightColor(uint32_t index);

    void setLightRadius(uint32_t index, const float& radius);

    float& getLightRadius(uint32_t index);

    Result updateUniformBuffer(platform::Context* context);

    rhi::UniformBuffer* getUniformBuffer();

  private:
    std::mutex mutex;
    bool dirty;

    struct UniformBufferObject
    {
        Light light[NUM_LIGHTS];
        uint32_t numLights;
    } ubo;

    rhi::UniformBuffer* uniformBuffer;

    static constexpr size_t uniformDataSize = sizeof(UniformBufferObject);
};
} // namespace scene