#pragma once

#include "core.h"
#include "resources.h"
#include "rhi/image.h"

namespace vk
{
class Context;
class DeviceMemory;
class Transition;
class RealBuffer;

// TODO
class Sampler final : public WrappedObject<Sampler, VkSampler>
{
  public:
    Sampler() = default;

    Result init(Context* context);

    void terminate(Context* context);

  private:
    VkResult create(VkDevice device, const VkSamplerCreateInfo& createInfo);
};

class ImageView final : public WrappedObject<ImageView, VkImageView>
{
  public:
    Result init(Context* context, VkImage image, Format format, VkComponentMapping components,
                VkImageSubresourceRange subresourceRange, VkImageViewType viewType);

    void terminate(Context* context);

  private:
    VkResult create(VkDevice device, const VkImageViewCreateInfo& createInfo);
};

class Image final : public rhi::Image, public WrappedObject<Image, VkImage>
{
  public:
    Image(rhi::DescriptorType descriptorType);

    ~Image() = default;

    Result init(rhi::Context* context, rhi::Format format, rhi::ImageType imageType, rhi::ImageUsageFlags imageUsage,
                rhi::MemoryPropertyFlags memoryProperty, uint32_t mipLevels, uint32_t layers, uint32_t samples,
                rhi::Extent3D extent) override;

    // For swapchain images
    Result init(Context* context, VkImage image, rhi::Format format, rhi::ImageType imageType,
                rhi::ImageUsageFlags imageUsage, uint32_t mipLevels, uint32_t layers, uint32_t samples,
                rhi::Extent3D extent);

    Result initView(Context* context, VkComponentMapping components, VkImageSubresourceRange subresourceRange);

    void terminate(rhi::Context* context) override;

    void release(Context* context); // Only for swapchain images

    Result update(rhi::Context* context, size_t size, void* data, std::vector<size_t>& mipOffsets) override;

    Result copy(Context* context, RealBuffer* srcBuffer, VkExtent3D extent, uint32_t mipLevel, uint32_t layer,
                size_t offset);

    Transition* updateImageLayoutAndBarrier(rhi::ImageLayout newLayout);

    VkImageSubresourceRange getWholeImageSubresourceRange();

  private:
    Result initImage(Context* context);

    VkResult create(VkDevice device, const VkImageCreateInfo& createInfo);

    const VkMemoryRequirements getMemoryRequirements(VkDevice device);

    VkResult bindMemory(VkDevice device);

  public:
    VkFormat getFormat();

    VkImageAspectFlags getAspectFlags();

    VkImageView getView();

    VkSampler getSampler();

    uint32_t getSamples();

    inline rhi::Extent3D getExtent()
    {
        return extent;
    }

  private:
    DeviceMemory* deviceMemory;
    uint32_t mipLevels;
    uint32_t layers;
    uint32_t samples;
    ImageView* view;
    Sampler* sampler;
    VkDescriptorImageInfo imageInfo;
};
} // namespace vk