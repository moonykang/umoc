#pragma once

#include "vulkan/core.h"

namespace vk
{
class Context;
class DeviceMemory;

class Image final : public WrappedObject<Image, VkImage>
{
  public:
    Image();

    ~Image() = default;

    Result init(Context* context, VkFormat format, uint32_t mipLevels, uint32_t layers, uint32_t samples,
                VkExtent3D extent, VkImageUsageFlags imageUsage, VkMemoryPropertyFlags memoryProperty);

    Result init(Context* context, VkImage image, VkFormat format, uint32_t mipLevels, uint32_t layers, uint32_t samples,
                VkExtent3D extent, VkImageUsageFlags imageUsage);

    void terminate(VkDevice device);

  private:
    Result initImage(Context* context);

    VkResult create(VkDevice device, const VkImageCreateInfo& createInfo);

    const VkMemoryRequirements getMemoryRequirements(VkDevice device);

  private:
    DeviceMemory* deviceMemory;
    VkFormat format;
    uint32_t mipLevels;
    uint32_t layers;
    uint32_t samples;
    VkExtent3D extent;
    VkImageUsageFlags imageUsage;
};
} // namespace vk