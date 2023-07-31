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
enum LightType
{
    LIGHT_TYPE_DIRECTIONAL,
    LIGHT_TYPE_POINT,
    LIGHT_TYPE_SPOT,
    LIGHT_TYPE_COUNT
};

struct Light
{
    Light()
    {
        set_light_radius(LIGHT_TYPE_POINT);
    }

    glm::vec4 data0;
    glm::vec4 data1;
    glm::vec4 data2;
    glm::vec4 data3;

    inline void set_light_direction(glm::vec3 value)
    {
        data0.x = value.x;
        data0.y = value.y;
        data0.z = value.z;
    }

    inline void set_light_position(glm::vec3 value)
    {
        data1.x = value.x;
        data1.y = value.y;
        data1.z = value.z;
    }

    inline void set_light_color(glm::vec3 value)
    {
        data2.x = value.x;
        data2.y = value.y;
        data2.z = value.z;
    }

    inline void set_light_intensity(float value)
    {
        data0.w = value;
    }

    inline void set_light_radius(float value)
    {
        data1.w = value;
    }

    inline void set_light_type(LightType value)
    {
        data3.x = value;
    }

    inline void set_light_cos_theta_outer(float value)
    {
        data3.y = value;
    }

    inline void set_light_cos_theta_inner(float value)
    {
        data3.z = value;
    }
};

class Lights
{
  public:
    Lights();

    virtual ~Lights() = default;

    Result init(platform::Context* context);

    void terminate(platform::Context* context);

    void setLightPosition(uint32_t index, const glm::vec3& position);

    void setLightColor(uint32_t index, const glm::vec3& color);

    void setLightRadius(uint32_t index, const float& radius);

    Result updateUniformBuffer(platform::Context* context);

    rhi::UniformBuffer* getUniformBuffer();

    void setLightNumber(uint32_t val);

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