#pragma once

#include "vulkan/core.h"
#include "vulkan/resources.h"

namespace vk
{
class Context;
class DeviceMemory;

class ImageView final : public WrappedObject<ImageView, VkImageView>
{
  public:
    Result init(Context* context, VkImage image, Format format, VkComponentMapping components,
                VkImageSubresourceRange subresourceRange, VkImageViewType viewType);

  private:
    VkResult create(VkDevice device, const VkImageViewCreateInfo& createInfo);
};

class Image final : public WrappedObject<Image, VkImage>
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

    void terminate(VkDevice device);

  private:
    Result initImage(Context* context);

    VkResult create(VkDevice device, const VkImageCreateInfo& createInfo);

    const VkMemoryRequirements getMemoryRequirements(VkDevice device);

  private:
    DeviceMemory* deviceMemory;
    Format format;
    VkImageType imageType;
    uint32_t mipLevels;
    uint32_t layers;
    uint32_t samples;
    VkExtent3D extent;
    VkImageUsageFlags imageUsage;
    ImageView view;
};

} // namespace vk