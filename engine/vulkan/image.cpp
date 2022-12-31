#include "vulkan/image.h"
#include "vulkan/context.h"
#include "vulkan/device.h"
#include "vulkan/memory.h"
#include "vulkan/physical_device.h"

namespace vk
{
Image::Image()
    : deviceMemory(nullptr), format(VK_FORMAT_UNDEFINED), mipLevels(0), layers(0), samples(1), extent(), imageUsage()
{
}

Result Image::init(Context* context, VkFormat format, uint32_t mipLevels, uint32_t layers, uint32_t samples,
                   VkExtent3D extent, VkImageUsageFlags imageUsage, VkMemoryPropertyFlags memoryProperty)
{
    Device* device = context->getDevice();

    this->format = format;
    this->mipLevels = mipLevels;
    this->layers = layers;
    this->samples = samples;
    this->extent = std::move(extent);
    this->imageUsage = imageUsage;

    try(initImage(context));

    const VkMemoryRequirements memoryRequirements = getMemoryRequirements(device->getHandle());

    deviceMemory = new DeviceMemory();
    try(deviceMemory->init(context, memoryRequirements, memoryProperty));

    return Result::Continue;
};

Result Image::init(VkImage image, VkFormat format, uint32_t mipLevels, uint32_t layers, uint32_t samples,
                   VkExtent3D extent, VkImageUsageFlags imageUsage)
{
    ASSERT(image != VK_NULL_HANDLE);

    this->format = format;
    this->mipLevels = mipLevels;
    this->layers = layers;
    this->samples = samples;
    this->extent = std::move(extent);
    this->imageUsage = imageUsage;

    return Result::Continue;
}

void Image::terminate(VkDevice device)
{
    if (valid())
    {
        vkDestroyImage(device, mHandle, nullptr);
    }

    DELETE(deviceMemory, device);
}

Result Image::initImage(Context* context)
{
    Device* device = context->getDevice();

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = format;
    imageCreateInfo.mipLevels = mipLevels;
    imageCreateInfo.arrayLayers = layers;
    imageCreateInfo.samples = VkSampleCountFlagBits(samples);
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.extent = {extent.width, extent.height, extent.depth};
    imageCreateInfo.usage = imageUsage;

    vk_try(create(device->getHandle(), imageCreateInfo));

    return Result::Continue;
}

VkResult Image::create(VkDevice device, const VkImageCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateImage(device, &createInfo, nullptr, &mHandle);
}

const VkMemoryRequirements Image::getMemoryRequirements(VkDevice device)
{
    ASSERT(valid());
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(device, mHandle, &memoryRequirements);
    return memoryRequirements;
}
} // namespace vk