#pragma once

#include "common/transform.h"
#include "common/util.h"
#include "ui/component.h"
#include <mutex>

constexpr uint32_t num_lights = 6;

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
    ALIGNED(16)
    glm::mat4 lightMatrix;
    ALIGNED(128)
    LightData light[NUM_LIGHTS];
    ALIGNED(1)
    uint32_t numLights;
};

class Light : public ui::Component
{
  public:
    Light() : dirty(true), color(1.0f)
    {
    }

    Result updateUI() override;

    util::Transform& getTransform()
    {
        return transform;
    }

  protected:
    util::Transform transform;

    glm::vec3 color;

    bool dirty;
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

    bool updateLightData(LightData& lightData, glm::mat4& lightMatrix)
    {
        if (dirty)
        {
            auto& m = transform.get();
            glm::vec3 direction = glm::normalize(glm::mat3(m) * glm::vec3(0.0f, -1.0f, 0.0f));
            glm::vec3 position = glm::vec3(m[3][0], m[3][1], m[3][2]);

            lightData.set_light_position(position);
            lightData.set_light_direction(direction);
            lightData.set_light_color(color);

            glm::mat4 viewMatrix = glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));
            lightMatrix = projection * viewMatrix;

            LOGD("Position %f %f %f", position.x, position.y, position.z);
            LOGD("Rotate %f %f %f", direction.x, direction.y, direction.z);
            LOGD("LightMatrix");
            LOGD("%f %f %f %f", lightMatrix[0][1], lightMatrix[0][1], lightMatrix[0][2], lightMatrix[0][3]);
            LOGD("%f %f %f %f", lightMatrix[1][1], lightMatrix[1][1], lightMatrix[1][2], lightMatrix[1][3]);
            LOGD("%f %f %f %f", lightMatrix[2][1], lightMatrix[2][1], lightMatrix[2][2], lightMatrix[2][3]);
            LOGD("%f %f %f %f", lightMatrix[3][1], lightMatrix[3][1], lightMatrix[3][2], lightMatrix[3][3]);

            dirty = false;

            return true;
        }

        return false;
    }

    void setProjection(float fov, float aspect, float near, float far)
    {
        projection = glm::perspective(glm::radians(fov), aspect, near, far);
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
