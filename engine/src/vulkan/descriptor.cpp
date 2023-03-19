#include "descriptor.h"
#include "buffer.h"
#include "commandBuffer.h"
#include "context.h"
#include "device.h"
#include "image.h"
#include "pendingState.h"
#include "pipeline.h"
#include "resources.h"

namespace vk
{
VkWriteDescriptorSet Buffer::getWriteDescriptorSet()
{
    VkWriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorType = convertToVkDescriptorType(type);
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.pBufferInfo = &bufferInfo;

    return writeDescriptorSet;
}

VkWriteDescriptorSet Image::getWriteDescriptorSet()
{
    VkWriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorType = convertToVkDescriptorType(type);
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.pImageInfo = &imageInfo;

    return writeDescriptorSet;
}

Result DescriptorSetLayout::init(Context* context, rhi::DescriptorInfoList& descriptorInfoList)
{
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

    for (auto& descriptorInfo : descriptorInfoList)
    {
        auto& descriptorSetLayoutBinding = descriptorSetLayoutBindings.emplace_back();
        descriptorSetLayoutBinding.binding = descriptorInfo.getBinding();
        descriptorSetLayoutBinding.descriptorType = convertToVkDescriptorType(descriptorInfo.getType());
        descriptorSetLayoutBinding.descriptorCount = 1;
        descriptorSetLayoutBinding.stageFlags = descriptorInfo.getShaderStage();
        descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
    descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();

    vk_try(create(context->getDevice()->getHandle(), descriptorSetLayoutCreateInfo));

    return Result::Continue;
}

void DescriptorSetLayout::terminate(Context* context)
{
    if (valid())
    {
        context->addGarbage(HandleType::DescriptorSetLayout, mHandle);
        mHandle = VK_NULL_HANDLE;
    }
}

rhi::DescriptorSet* Context::allocateDescriptorSet()
{
    return new DescriptorSet();
}

DescriptorSet::DescriptorSet() : layout(nullptr)
{
}

Result DescriptorSet::init(rhi::Context* rhiContext, rhi::DescriptorInfoList& descriptorInfoList)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);

    layout = new DescriptorSetLayout();
    layout->init(context, descriptorInfoList);

    const uint32_t descriptorSetCount = 1;
    VkDescriptorSetLayout descriptorSetLayouts[descriptorSetCount] = {layout->getHandle()};
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;
    descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts;

    DescriptorPool* descriptorPool = context->getDescriptorPool();

    mHandle = context->getDescriptorPool()->allocate(context, descriptorSetAllocateInfo);

    return Result::Continue;
}

void DescriptorSet::terminate(rhi::Context* rhiContext)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);
    if (valid())
    {
        mHandle = VK_NULL_HANDLE;
    }
    TERMINATE(layout, context);
}

// TODO
/*
    Sampler = 0,
    Combined_Image_Sampler = 1,
    Sampled_Image = 2,
    Storage_Image = 3,
    Uniform_Texel_Buffer = 4,
    Storage_Texel_Buffer = 5,
    Uniform_Buffer = 6,
    Storage_Buffer = 7,
    Uniform_Buffer_Dynamic = 8,
    Storage_Buffer_Dynamic = 9,
    Input_Attachment = 10,
    Acceleration_structure = 11,
*/
Result DescriptorSet::update(rhi::Context* rhiContext, rhi::DescriptorList descriptors, std::vector<uint32_t>& offsets)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);
    std::vector<VkWriteDescriptorSet> writeDescriptorSets;

    dynamicOffsets = std::move(offsets);

    for (auto& descriptorPair : descriptors)
    {
        auto& descriptorInfo = descriptorPair.first;
        auto descriptor = descriptorPair.second;

        rhi::DescriptorType type = descriptorInfo.getType();

        switch (type)
        {
        case rhi::DescriptorType::Uniform_Buffer:
        case rhi::DescriptorType::Storage_Buffer:
        case rhi::DescriptorType::Uniform_Buffer_Dynamic:
        case rhi::DescriptorType::Storage_Buffer_Dynamic: {
            Buffer* buffer = reinterpret_cast<Buffer*>(descriptor);
            writeDescriptorSets.push_back(buffer->getWriteDescriptorSet());

            auto& writeDescriptorSet = writeDescriptorSets.back();
            writeDescriptorSet.dstSet = mHandle;
            writeDescriptorSet.dstBinding = descriptorInfo.getBinding();
            writeDescriptorSet.descriptorCount = 1;
            break;
        }
        case rhi::DescriptorType::Sampler:
        case rhi::DescriptorType::Combined_Image_Sampler:
        case rhi::DescriptorType::Sampled_Image:
        case rhi::DescriptorType::Storage_Image:
        case rhi::DescriptorType::Uniform_Texel_Buffer:
        case rhi::DescriptorType::Storage_Texel_Buffer:
        case rhi::DescriptorType::Input_Attachment:
        case rhi::DescriptorType::Acceleration_structure:
            Image* image = reinterpret_cast<Image*>(descriptor);
            writeDescriptorSets.push_back(image->getWriteDescriptorSet());

            auto& writeDescriptorSet = writeDescriptorSets.back();
            writeDescriptorSet.dstSet = mHandle;
            writeDescriptorSet.dstBinding = descriptorInfo.getBinding();
            break;
        }
    }

    if (!writeDescriptorSets.empty())
    {
        vkUpdateDescriptorSets(context->getDevice()->getHandle(), static_cast<uint32_t>(writeDescriptorSets.size()),
                               writeDescriptorSets.data(), 0, nullptr);
    }
    return Result::Continue;
}

void DescriptorSet::bind(rhi::Context* rhiContext, uint32_t binding)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);
    Pipeline* pipeline = context->getPendingState()->getPipeline();

    CommandBuffer* commandBuffer = context->getActiveCommandBuffer();

    // minUniformBufferOffsetAlignment
    commandBuffer->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getLayout()->getHandle(), binding, 1,
                                      &mHandle, static_cast<uint32_t>(dynamicOffsets.size()), dynamicOffsets.data());
}

DescriptorSetLayout* DescriptorSet::getLayout()
{
    ASSERT(layout)
    return layout;
}

VkResult DescriptorSetLayout::create(VkDevice device, const VkDescriptorSetLayoutCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &mHandle);
}

DescriptorPool* Context::getDescriptorPool()
{
    return descriptorPool;
}

Result DescriptorPool::init(Context* context)
{
    std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
    {
        auto& descriptorPoolSize = descriptorPoolSizes.emplace_back();
        descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorPoolSize.descriptorCount = 1000;
    }

    {
        auto& descriptorPoolSize = descriptorPoolSizes.emplace_back();
        descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        descriptorPoolSize.descriptorCount = 1000;
    }

    {
        auto& descriptorPoolSize = descriptorPoolSizes.emplace_back();
        descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorPoolSize.descriptorCount = 1000;
    }

    {
        auto& descriptorPoolSize = descriptorPoolSizes.emplace_back();
        descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        descriptorPoolSize.descriptorCount = 1000;
    }

    {
        auto& descriptorPoolSize = descriptorPoolSizes.emplace_back();
        descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        descriptorPoolSize.descriptorCount = 1000;
    }

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
    descriptorPoolCreateInfo.maxSets = 5000;

    vk_try(create(context->getDevice()->getHandle(), descriptorPoolCreateInfo));
    return Result::Continue;
}

VkResult DescriptorPool::create(VkDevice device, const VkDescriptorPoolCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateDescriptorPool(device, &createInfo, nullptr, &mHandle);
}

void DescriptorPool::terminate(Context* context)
{
    if (valid())
    {
        context->addGarbage(HandleType::DescriptorPool, mHandle);
        mHandle = VK_NULL_HANDLE;
    }
}

VkDescriptorSet DescriptorPool::allocate(Context* context, VkDescriptorSetAllocateInfo& allocateInfo)
{
    ASSERT(valid());

    VkDescriptorSet descriptorSetOut = descriptorSets.emplace_back();
    allocateInfo.descriptorPool = mHandle;

    call(allocateDescriptorSets(context->getDevice()->getHandle(), allocateInfo, &descriptorSetOut));

    ASSERT(descriptorSetOut != VK_NULL_HANDLE);

    return descriptorSetOut;
}

VkResult DescriptorPool::allocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo& allocateInfo,
                                                VkDescriptorSet* pDescriptorSetOut)
{
    ASSERT(valid());
    return vkAllocateDescriptorSets(device, &allocateInfo, pDescriptorSetOut);
}
} // namespace vk
