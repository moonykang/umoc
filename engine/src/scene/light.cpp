#include "light.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"

namespace scene
{
Result Light::updateUI()
{
    bool changed = false;

    ImGui::Text("Directional Light");

    changed |= ImGui::SliderFloat3("Position", &position.x, -10, 30);
    changed |= ImGui::SliderFloat3("Rotation", &vRotate.x, -180.f, 180.f);
    changed |= ImGui::SliderFloat3("Color", &color.x, 0.0f, 1.0f);

    if (changed)
    {
        dirty = true;
    }
    return Result::Continue;
}

bool DirectionalLight::updateLightData(LightData& lightData, glm::mat4& lightMatrix)
{
    if (dirty)
    {
        auto& m = get();

        auto inv = glm::inverse(m);
        // glm::vec3 direction = glm::normalize(glm::mat3(m) * glm::vec3(0.0f, 0.0f, 1.0f));
        // glm::vec3 position = glm::vec3(m[3][0], m[3][1], m[3][2]);
        glm::vec3 lookat = glm::vec3(0, 0, 0);

        glm::vec3 direction = glm::normalize(lookat - position);

        lightData.set_light_type(LightType::LIGHT_TYPE_DIRECTIONAL);
        lightData.set_light_position(position);
        lightData.set_light_direction(direction);
        lightData.set_light_color(color);

        //   glm::mat4 viewMatrix = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 viewMatrix = glm::lookAt(-position, lookat, glm::vec3(0.0f, 1.0f, 0.0f));
        lightMatrix = projection * viewMatrix;

        LOGD("Position %f %f %f", position.x, position.y, position.z);
        LOGD("Direction %f %f %f", direction.x, direction.y, direction.z);
        LOGD("LightMatrix");
        LOGD("%f %f %f %f", lightMatrix[0][0], lightMatrix[0][1], lightMatrix[0][2], lightMatrix[0][3]);
        LOGD("%f %f %f %f", lightMatrix[1][0], lightMatrix[1][1], lightMatrix[1][2], lightMatrix[1][3]);
        LOGD("%f %f %f %f", lightMatrix[2][0], lightMatrix[2][1], lightMatrix[2][2], lightMatrix[2][3]);
        LOGD("%f %f %f %f", lightMatrix[3][0], lightMatrix[3][1], lightMatrix[3][2], lightMatrix[3][3]);

        LOGD("LightView");
        LOGD("%f %f %f %f", viewMatrix[0][0], viewMatrix[0][1], viewMatrix[0][2], viewMatrix[0][3]);
        LOGD("%f %f %f %f", viewMatrix[1][0], viewMatrix[1][1], viewMatrix[1][2], viewMatrix[1][3]);
        LOGD("%f %f %f %f", viewMatrix[2][0], viewMatrix[2][1], viewMatrix[2][2], viewMatrix[2][3]);
        LOGD("%f %f %f %f", viewMatrix[3][0], viewMatrix[3][1], viewMatrix[3][2], viewMatrix[3][3]);

        dirty = false;

        return true;
    }

    return false;
}

Lights::Lights() : uniformBuffer(nullptr), dirty(false)
{
    ubo.numLights = 1;
    ubo.lightMatrix = glm::mat4(1.0f);

    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        ubo.light[i] = LightData();
    }
}

Result Lights::init(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    uniformBuffer = context->allocateUniformBuffer(uniformDataSize, &ubo);

    return Result::Continue;
}

void Lights::terminate(platform::Context* platformContext)
{
}

void Lights::setLightPosition(uint32_t index, const glm::vec3& position)
{
    ASSERT(index < NUM_LIGHTS);
    ubo.light[index].set_light_position(position);
    dirty = true;
}

void Lights::setLightColor(uint32_t index, const glm::vec3& color)
{
    ASSERT(index < NUM_LIGHTS);
    ubo.light[index].set_light_color(color);
    dirty = true;
}

void Lights::setLightRadius(uint32_t index, const float& radius)
{
    ASSERT(index < NUM_LIGHTS);
    ubo.light[index].set_light_radius(radius);
    dirty = true;
}

Result Lights::updateUniformBuffer(platform::Context* platformContext)
{
    // updateView();

    std::lock_guard<std::mutex> lock(mutex);

    dirty |= directionalLight.updateLightData(ubo.light[0], ubo.lightMatrix);

    if (dirty)
    {
        if (false)
        {
            LOGD("Debug light ubo, numLights %u, uniformDataSize %u", ubo.numLights, uniformDataSize);

            for (uint32_t i = 0; i < NUM_LIGHTS; i++)
            {
                auto light = ubo.light[i];
                LOGD("Light%u", i);
                LOGD("%f %f %f %f", light.data0.x, light.data0.y, light.data0.z, light.data0.w);
                LOGD("%f %f %f %f", light.data1.x, light.data1.y, light.data1.z, light.data1.w);
                LOGD("%f %f %f %f", light.data2.x, light.data2.y, light.data2.z, light.data2.w);
                LOGD("%f %f %f %f", light.data3.x, light.data3.y, light.data3.z, light.data3.w);
            }
        }
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
        try(uniformBuffer->update(context, uniformDataSize, &ubo));
        dirty = false;
    }

    return Result::Continue;
}

rhi::SubAllocatedBuffer* Lights::getUniformBuffer()
{
    return uniformBuffer;
}

void Lights::setLightNumber(uint32_t val)
{
    ubo.numLights = val;
    dirty = true;
}

Result Lights::updateUI()
{
    try(directionalLight.updateUI());

    return Result::Continue;
}
} // namespace scene
