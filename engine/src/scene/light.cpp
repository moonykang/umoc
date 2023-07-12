#include "light.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"

namespace scene
{
Light::Light() : position(glm::vec4(0.0f)), color(glm::vec4(1.0f)), radius(1.0f)
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

void Lights::setLightPosition(uint32_t index, const glm::vec4& position)
{
    ASSERT(index < NUM_LIGHTS);
    ubo.light[index].position = position;
    dirty = true;
}

glm::vec4& Lights::getLightPosition(uint32_t index)
{
    return ubo.light[index].position;
}

void Lights::setLightColor(uint32_t index, const glm::vec3& color)
{
    ASSERT(index < NUM_LIGHTS);
    ubo.light[index].color = color;
    dirty = true;
}

glm::vec3& Lights::getLightColor(uint32_t index)
{
    return ubo.light[index].color;
}

void Lights::setLightRadius(uint32_t index, const float& radius)
{
    ASSERT(index < NUM_LIGHTS);
    ubo.light[index].radius = radius;
    dirty = true;
}

float& Lights::getLightRadius(uint32_t index)
{
    return ubo.light[index].radius;
}

Result Lights::updateUniformBuffer(platform::Context* platformContext)
{
    // updateView();

    std::lock_guard<std::mutex> lock(mutex);

    if (dirty)
    {
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
} // namespace scene
