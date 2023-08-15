#include "light.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"

namespace scene
{
Result Light::updateUI()
{
    bool changed = false;

    auto& position = transform.getPosition();
    auto& rotation = transform.getRotation();

    ImGui::Text("Directional Light");
    changed |= ImGui::SliderFloat3("Position", &transform.position.x, -100, 100);
    changed |= ImGui::SliderFloat3("Rotation", &rotation.x, -360, 360);

    if (changed)
    {
        transform.updateExternal();

        auto m = transform.get();
        transform.debug();

        dirty = true;
    }
    return Result::Continue;
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
