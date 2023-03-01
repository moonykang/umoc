#include "descriptor.h"
#include "context.h"
#include "device.h"
#include "resources.h"

namespace vk
{
rhi::DescriptorSetLayout* Context::allocateDescriptorSetLayout()
{
    return new DescriptorSetLayout();
}

Result DescriptorSetLayout::init(rhi::Context* rhiContext, rhi::DescriptorInfoList& descriptorInfoList)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);

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

void DescriptorSetLayout::terminate(rhi::Context* rhiContext)
{
    if (valid())
    {
        Context* context = reinterpret_cast<Context*>(rhiContext);
        context->addGarbage(HandleType::DescriptorSetLayout, mHandle);
        mHandle = VK_NULL_HANDLE;
    }
}

VkResult DescriptorSetLayout::create(VkDevice device, const VkDescriptorSetLayoutCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &mHandle);
}
} // namespace vk