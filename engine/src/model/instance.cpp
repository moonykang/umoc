#include "instance.h"

#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"
#include "rhi/shader.h"
#include "vertexInput.h"

namespace model
{
Instance::Instance() : vertexInput(nullptr), uniformBuffer(nullptr), initialized(false)
{
}

Result Instance::init(platform::Context* platformContext, VertexInput* vertexInput)
{
    std::lock_guard<std::mutex> lock(mutex);

    if (!initialized)
    {
        rhi::Context* context = platformContext->getRHI();

        this->vertexInput = vertexInput;

        uniformBuffer = context->allocateUniformBuffer(sizeof(UniformBufferObject), &ubo);

        descriptorSet = context->allocateDescriptorSet();

        rhi::DescriptorInfoList descriptorInfoList;
        descriptorInfoList.push_back({0, rhi::ShaderStage::Vertex, rhi::DescriptorType::Uniform_Buffer_Dynamic});
        descriptorSet->init(context, descriptorInfoList);

        initialized = true;
    }

    return Result::Continue;
}

void Instance::terminate(platform::Context* platformContext)
{
    rhi::Context* context = platformContext->getRHI();

    TERMINATE(descriptorSet, context);

    uniformBuffer = nullptr;
    vertexInput = nullptr;
    initialized = false;
}

Result Instance::updateUniformBuffer(platform::Context* platformContext)
{
    // std::lock_guard<std::mutex> lock(mutex);

    // if (dirty)
    {
        rhi::Context* context = reinterpret_cast<rhi::Context*>(platformContext);
        try(uniformBuffer->update(context, sizeof(UniformBufferObject), &ubo));

        rhi::DescriptorList descriptorList;
        descriptorList.push_back({{0, rhi::ShaderStage::Vertex, rhi::DescriptorType::Uniform_Buffer_Dynamic},
                                  uniformBuffer->getDescriptor()});
        try(descriptorSet->update(context, descriptorList));
    }

    return Result::Continue;
}
} // namespace model