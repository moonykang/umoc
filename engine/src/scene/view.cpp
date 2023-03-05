#include "view.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/defines.h"
#include "rhi/descriptor.h"

namespace scene
{
View::View() : dirty(true), uniformBuffer(nullptr), descriptorSet(nullptr)
{
}

View::~View()
{
}

Result View::init(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);

    uniformBuffer = context->allocateUniformBuffer(uniformDataSize, &ubo);

    descriptorSet = context->allocateDescriptorSet();

    rhi::DescriptorInfoList descriptorInfoList;
    descriptorInfoList.push_back({0, rhi::ShaderStage::Vertex, rhi::DescriptorType::Uniform_Buffer_Dynamic});

    try(descriptorSet->init(context, descriptorInfoList));

    return Result::Continue;
}

void View::terminate(platform::Context* platformContext)
{
    rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
    TERMINATE(descriptorSet, context);
}

void View::setView(glm::vec3 position, glm::vec3 lookAt, glm::vec3 headUp)
{
    ubo.view = glm::lookAt(position, lookAt, headUp);

    // lock
    {
        std::lock_guard<std::mutex> lock(mutex);
        dirty = true;
    }
}

void View::setPerspective(float fov, float ratio, float minDepth, float maxDepth)
{
    ubo.perspective = glm::perspective(glm::radians(fov), ratio, minDepth, maxDepth);
    ubo.perspective[1][1] *= -1;

    // lock
    {
        std::lock_guard<std::mutex> lock(mutex);
        dirty = true;
    }
}

Result View::updateUniformBuffer(platform::Context* platformContext)
{
    std::lock_guard<std::mutex> lock(mutex);

    if (dirty)
    {
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
        try(uniformBuffer->update(context, uniformDataSize, &ubo));

        rhi::DescriptorList descriptorList;
        descriptorList.push_back({{0, rhi::ShaderStage::Vertex, rhi::DescriptorType::Uniform_Buffer_Dynamic},
                                  uniformBuffer->getDescriptor()});
        try(descriptorSet->update(context, descriptorList));
    }

    return Result::Continue;
}
} // namespace scene