#pragma once
#include "core.h"
#include "rhi/descriptor.h"

namespace vk
{
class Context;

class DescriptorSetLayout final : public rhi::DescriptorSetLayout,
                                  public WrappedObject<DescriptorSetLayout, VkDescriptorSetLayout>
{
  public:
    Result init(rhi::Context* context, rhi::DescriptorInfoList& descriptorInfoList) override;

    void terminate(rhi::Context* context) override;

  private:
    VkResult create(VkDevice device, const VkDescriptorSetLayoutCreateInfo& createInfo);
};
} // namespace vk