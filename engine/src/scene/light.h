#pragma once

#include "common/transform.h"
#include "common/util.h"
#include "ui/component.h"
#include <mutex>

#define NUM_LIGHTS 8

namespace rhi
{
class SubAllocatedBuffer;
} // namespace rhi

namespace platform
{
class Context;
}

namespace scene
{
enum LightType
{
    LIGHT_TYPE_NONE,
    LIGHT_TYPE_DIRECTIONAL,
    LIGHT_TYPE_POINT,
    LIGHT_TYPE_SPOT,
    LIGHT_TYPE_COUNT
};

struct LightData
{
    LightData()
    {
        memset(this, 0, sizeof(LightData));

        set_light_type(LIGHT_TYPE_POINT);
        set_light_intensity(1.0f);
        set_light_color(glm::vec3(1.0f));
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

struct LightUniformBuffer
{
    ALIGNED(128)
    LightData light[NUM_LIGHTS];
    ALIGNED(16)
    glm::mat4 lightMatrix;
    ALIGNED(4)
    uint32_t numLights;
};

class Light : public ui::Component, public util::Transform
{
  public:
    Light() : util::Transform(util::TransformType::LookAt), color(1.0f)
    {
    }

    Result updateUI() override;

    void setColor(glm::vec3 v)
    {
        color = v;
        dirty = true;
    }

    void setPosition(glm::vec3 v)
    {
        position = v;
        dirty = true;
    }

  protected:
    glm::vec3 color;
};

class DirectionalLight : public Light
{
  public:
    DirectionalLight() : enabled(false), projection(1.0f)
    {
    }

    ~DirectionalLight()
    {
    }

    bool updateLightData(LightData& lightData, glm::mat4& lightMatrix);

    void setProjection(float fov, float aspect, float near, float far)
    {
        projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near, far);
        // projection = glm::perspective(glm::radians(fov), aspect, near, far);
        dirty = true;
    }

  private:
    bool enabled;
    glm::mat4 projection;
};

class PointLight
{
};

class SpotLight
{
};

class Lights : public ui::Component
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

    rhi::SubAllocatedBuffer* getUniformBuffer();

    void setLightNumber(uint32_t val);

    DirectionalLight& getDirectionalLight()
    {
        return directionalLight;
    }

    Result updateUI() override;

  private:
    std::mutex mutex;
    bool dirty;

    LightUniformBuffer ubo;
    DirectionalLight directionalLight;
    std::vector<PointLight> pointLights;
    std::vector<SpotLight> spotLights;

    rhi::SubAllocatedBuffer* uniformBuffer;

    static constexpr size_t uniformDataSize = sizeof(LightUniformBuffer);
};
} // namespace scene
