#pragma once

#include "core.h"
#include "resources.h"
#include "rhi/image.h"

namespace vk
{
class Context;
class DeviceMemory;
class Transition;

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
    Image();

    ~Image() = default;

    Result init(Context* context, Format format, VkImageType type, uint32_t mipLevels, uint32_t layers,
                uint32_t samples, VkExtent3D extent, VkImageUsageFlags imageUsage,
                VkMemoryPropertyFlags memoryProperty);

    // For swapchain images
    Result init(Context* context, VkImage image, Format format, VkImageType type, uint32_t mipLevels, uint32_t layers,
                uint32_t samples, VkExtent3D extent, VkImageUsageFlags imageUsage);

    Result initView(Context* context);

    Result initView(Context* context, VkComponentMapping components, VkImageSubresourceRange subresourceRange);

    void terminate(Context* context);

    void release(Context* context); // Only for swapchain images

    Transition* updateImageLayoutAndBarrier(rhi::ImageLayout newLayout);

    VkImageSubresourceRange getWholeImageSubresourceRange();

  private:
    Result initImage(Context* context);

    VkResult create(VkDevice device, const VkImageCreateInfo& createInfo);

    const VkMemoryRequirements getMemoryRequirements(VkDevice device);

  public:
    VkFormat getFormat();

    VkImageView getView();

    uint32_t getSamples();

    inline VkExtent3D getExtent()
    {
        return extent;
    }

  private:
    DeviceMemory* deviceMemory;
    Format format;
    VkImageType imageType;
    uint32_t mipLevels;
    uint32_t layers;
    uint32_t samples;
    VkExtent3D extent;
    VkImageUsageFlags imageUsage;
    ImageView* view;
    rhi::ImageLayout imageLayout;
};

} // namespace vk