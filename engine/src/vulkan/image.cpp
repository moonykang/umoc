#include "vulkan/image.h"
#include "vulkan/context.h"
#include "vulkan/device.h"
#include "vulkan/memory.h"
#include "vulkan/physicalDevice.h"

namespace vk
{

Result ImageView::init(Context* context, VkImage image, Format format, VkComponentMapping components,
                       VkImageSubresourceRange subresourceRange, VkImageViewType viewType)
{
    Device* device = context->getDevice();

    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.pNext = NULL;
    imageViewCreateInfo.format = format.format;
    imageViewCreateInfo.components = components;
    imageViewCreateInfo.subresourceRange = subresourceRange;
    imageViewCreateInfo.viewType = viewType;
    imageViewCreateInfo.flags = 0;
    imageViewCreateInfo.image = image;

    vk_try(create(device->getHandle(), imageViewCreateInfo));

    return Result::Continue;
}

VkResult ImageView::create(VkDevice device, const VkImageViewCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateImageView(device, &createInfo, nullptr, &mHandle);
}

Image::Image()
    : deviceMemory(nullptr), format({VK_FORMAT_UNDEFINED, VK_IMAGE_ASPECT_NONE}), imageType(VK_IMAGE_TYPE_2D),
      mipLevels(0), layers(0), samples(1), extent(), imageUsage()
{
}

Result Image::init(Context* context, Format format, VkImageType imageType, uint32_t mipLevels, uint32_t layers,
                   uint32_t samples, VkExtent3D extent, VkImageUsageFlags imageUsage,
                   VkMemoryPropertyFlags memoryProperty)
{
    Device* device = context->getDevice();

    this->format = format;
    this->imageType = imageType;
    this->mipLevels = mipLevels;
    this->layers = layers;
    this->samples = samples;
    this->extent = std::move(extent);
    this->imageUsage = imageUsage;

    try(initImage(context));

    const VkMemoryRequirements memoryRequirements = getMemoryRequirements(device->getHandle());

    deviceMemory = new DeviceMemory();
    try(deviceMemory->init(context, memoryRequirements, memoryProperty));

    VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
    if (extent.depth > 1)
    {
        viewType = VK_IMAGE_VIEW_TYPE_3D;
    }
    else if (layers > 1)
    {
        viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    }

    VkComponentMapping components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,
                                     VK_COMPONENT_SWIZZLE_A};

    VkImageSubresourceRange subresourceRange = {format.aspects, 0, 1, 0, 1};

    return Result::Continue;
};

Result Image::init(Context* context, VkImage image, Format format, VkImageType imageType, uint32_t mipLevels,
                   uint32_t layers, uint32_t samples, VkExtent3D extent, VkImageUsageFlags imageUsage)
{
    ASSERT(image != VK_NULL_HANDLE);

    this->format = format;
    this->imageType = imageType;
    this->mipLevels = mipLevels;
    this->layers = layers;
    this->samples = samples;
    this->extent = std::move(extent);
    this->imageUsage = imageUsage;

    VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
    if (extent.depth > 1)
    {
        viewType = VK_IMAGE_VIEW_TYPE_3D;
    }
    else if (layers > 1)
    {
        viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    }

    VkComponentMapping components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,
                                     VK_COMPONENT_SWIZZLE_A};

    VkImageSubresourceRange subresourceRange = {format.aspects, 0, 1, 0, 1};

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
    imageCreateInfo.imageType = imageType;
    imageCreateInfo.format = format.format;
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

Result Image::initView(Context* context)
{
    VkComponentMapping components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,
                                     VK_COMPONENT_SWIZZLE_A};

    VkImageSubresourceRange subresourceRange = {format.aspects, 0, mipLevels, 0, layers};

    return initView(context, components, subresourceRange);
}

Result Image::initView(Context* context, VkComponentMapping components, VkImageSubresourceRange subresourceRange)
{
    ASSERT(valid());

    VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;

    switch (imageType)
    {
    case VK_IMAGE_TYPE_1D:
        viewType = layers > 0 ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
        break;
    case VK_IMAGE_TYPE_2D:
        viewType = layers > 0 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
        break;
    case VK_IMAGE_TYPE_3D:
        viewType = VK_IMAGE_VIEW_TYPE_3D;
        break;
    default:
        UNREACHABLE();
    }

    return view.init(context, mHandle, format, components, subresourceRange, viewType);
}

VkFormat Image::getFormat()
{
    ASSERT(valid());
    return format.format;
}

VkImageView Image::getView()
{
    ASSERT(valid() && view.valid());
    return view.getHandle();
}

uint32_t Image::getSamples()
{
    ASSERT(valid());
    return samples;
}
} // namespace vk