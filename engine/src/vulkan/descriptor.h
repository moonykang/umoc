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

class DescriptorSet final : public rhi::DescriptorSet, public WrappedObject<DescriptorSet, VkDescriptorSet>
{
  public:
    Result init(rhi::Context* context, rhi::DescriptorSetLayout* descriptorSetLayout) override;

    void terminate(rhi::Context* context) override;

    Result update(rhi::Context* context, rhi::DescriptorList descriptors) override;

    void bind(rhi::Context* context, uint32_t binding) override;

  private:
};

class DescriptorPool final : public WrappedObject<DescriptorPool, VkDescriptorPool>
{
  public:
    Result init(Context* context);

    void terminate(Context* context);

    VkDescriptorSet allocate(Context* context, VkDescriptorSetAllocateInfo& allocateInfo);

  private:
    VkResult create(VkDevice device, const VkDescriptorPoolCreateInfo& createInfo);

    VkResult allocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo& allocateInfo,
                                    VkDescriptorSet* pDescriptorSetOut);

  private:
    std::vector<VkDescriptorSet> descriptorSets;
};
} // namespace vk