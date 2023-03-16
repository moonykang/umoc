#include "instance.h"

#include "material.h"
#include "rhi/buffer.h"
#include "rhi/context.h"
#include "rhi/descriptor.h"
#include "rhi/shader.h"
#include "vertexInput.h"

namespace model
{
Instance::Instance(Object* object, Instance* instance, Material* material, uint32_t firstIndex, uint32_t indexCount,
                   uint32_t firstVertex, uint32_t vertexCount, glm::mat4 transform)
    : object(object), prevInstance(instance), material(material), firstIndex(firstIndex), indexCount(indexCount),
      firstVertex(firstVertex), vertexCount(vertexCount), ubo({transform}), uniformBuffer(nullptr),
      descriptorSet(nullptr), initialized(false)
{
}

Result Instance::init(platform::Context* platformContext)
{
    std::lock_guard<std::mutex> lock(mutex);

    if (!initialized)
    {
        rhi::Context* context = platformContext->getRHI();

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

    RELEASE(object);
    RELEASE(uniformBuffer);

    TERMINATE(prevInstance, platformContext);

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

void Instance::draw(platform::Context* platformContext)
{
    rhi::Context* context = platformContext->getRHI();

    context->drawIndexed(indexCount, 1, firstIndex, 0, 0);

    if (prevInstance != nullptr)
    {
        prevInstance->draw(context);
    }
}

Material* Instance::getMaterial()
{
    return material;
}
} // namespace model