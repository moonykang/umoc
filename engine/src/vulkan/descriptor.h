#pragma once
#include "core.h"
#include "rhi/descriptor.h"

namespace vk
{
class Context;

class DescriptorSetLayout final : public WrappedObject<DescriptorSetLayout, VkDescriptorSetLayout>
{
  public:
    Result init(Context* context, rhi::DescriptorInfoList& descriptorInfoList);

    void terminate(Context* context);

  private:
    VkResult create(VkDevice device, const VkDescriptorSetLayoutCreateInfo& createInfo);
};

class DescriptorSet final : public rhi::DescriptorSet, public WrappedObject<DescriptorSet, VkDescriptorSet>
{
  public:
    DescriptorSet();

    Result init(rhi::Context* context, rhi::DescriptorInfoList& descriptorInfoList) override;

    void terminate(rhi::Context* context) override;

    Result update(rhi::Context* context, rhi::DescriptorList descriptors, std::vector<uint32_t>& offsets) override;

    void bind(rhi::Context* context, uint32_t binding) override;

    DescriptorSetLayout* getLayout();

  private:
    DescriptorSetLayout* layout;
    std::vector<uint32_t> dynamicOffsets;
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