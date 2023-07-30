#include "light.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"

namespace scene
{
Light::Light() : data0(0.f), data1(0.f), data2(0.f), data3(0.f)
{
}

Lights::Lights() : uniformBuffer(nullptr), dirty(false)
{
    ubo.numLights = 1;
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

    if (dirty)
    {
        // TODO
        ubo.view = glm::lookAt(directionalLight.position, directionalLight.direction, glm::vec3(0.f, 1.f, 0.f));

        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
        try(uniformBuffer->update(context, uniformDataSize, &ubo));
        dirty = false;
    }

    return Result::Continue;
}

rhi::UniformBuffer* Lights::getUniformBuffer()
{
    return uniformBuffer;
}

void Lights::setPerspective(float fov, float ratio, float minDepth, float maxDepth)
{
    std::lock_guard<std::mutex> lock(mutex);

    dirty = true;

    ubo.projection = glm::ortho(glm::radians(fov), ratio, minDepth, maxDepth);
}

void Lights::setDirectionalLightPosition(glm::vec3 position)
{
    std::lock_guard<std::mutex> lock(mutex);

    dirty = true;

    directionalLight.position = position;
}

void Lights::setDirectionalLightDirection(glm::vec3 direction)
{
    std::lock_guard<std::mutex> lock(mutex);

    dirty = true;
    directionalLight.direction = direction;
}

Light& Lights::getLight(uint32_t index)
{
    return ubo.light[index];
}

void Lights::setNumLights(uint32_t num)
{
    ubo.numLights = num;
}
} // namespace scene
