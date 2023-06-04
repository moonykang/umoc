#include "light.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"

namespace scene
{
Light::Light() : uniformBuffer(nullptr), dirty(false)
{
}

Result Light::init(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    uniformBuffer = context->allocateUniformBuffer(uniformDataSize, &ubo);

    return Result::Continue;
}

void Light::terminate(platform::Context* platformContext)
{
}

void Light::setLight(glm::vec4 position)
{
    ubo.position = position;
    dirty = true;
}

Result Light::updateUniformBuffer(platform::Context* platformContext)
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

rhi::UniformBuffer* Light::getUniformBuffer()
{
    return uniformBuffer;
}
} // namespace scene