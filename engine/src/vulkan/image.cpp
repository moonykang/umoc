#include "image.h"
#include "context.h"
#include "device.h"
#include "memory.h"
#include "physicalDevice.h"
#include "transition.h"

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

void ImageView::terminate(Context* context)
{
    if (valid())
    {
        context->addGarbage(HandleType::ImageView, mHandle);
        mHandle = VK_NULL_HANDLE;
    }
}

VkResult ImageView::create(VkDevice device, const VkImageViewCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateImageView(device, &createInfo, nullptr, &mHandle);
}

Image::Image()
    : deviceMemory(nullptr), format({VK_FORMAT_UNDEFINED, VK_IMAGE_ASPECT_NONE}), imageType(VK_IMAGE_TYPE_2D),
      mipLevels(0), layers(0), samples(1), extent(), imageUsage(), view(nullptr),
      imageLayout(rhi::ImageLayout::Undefined)
{
}

/*
typedef struct VkImageSubresourceRange {
    VkImageAspectFlags    aspectMask;
    uint32_t              baseMipLevel;
    uint32_t              levelCount;
    uint32_t              baseArrayLayer;
    uint32_t              layerCount;
} VkImageSubresourceRange;
*/
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

    VkImageSubresourceRange subresourceRange = {format.aspects, 0, mipLevels, 0, layers};

    return Result::Continue;
};

Result Image::init(Context* context, VkImage image, Format format, VkImageType imageType, uint32_t mipLevels,
                   uint32_t layers, uint32_t samples, VkExtent3D extent, VkImageUsageFlags imageUsage)
{
    ASSERT(image != VK_NULL_HANDLE && this->mHandle == VK_NULL_HANDLE);

    this->format = format;
    this->imageType = imageType;
    this->mipLevels = mipLevels;
    this->layers = layers;
    this->samples = samples;
    this->extent = std::move(extent);
    this->imageUsage = imageUsage;
    this->mHandle = image;

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

    VkImageSubresourceRange subresourceRange = {format.aspects, 0, mipLevels, 0, layers};

    return Result::Continue;
}

void Image::terminate(Context* context)
{
    if (valid())
    {
        context->addGarbage(HandleType::Image, mHandle);
        mHandle = VK_NULL_HANDLE;
    }

    TERMINATE(view, context);
    TERMINATE(deviceMemory, context);
}

void Image::release(Context* context)
{
    TERMINATE(view, context);
    TERMINATE(deviceMemory, context);
    mHandle = VK_NULL_HANDLE;
}

Transition* Image::updateImageLayoutAndBarrier(rhi::ImageLayout newLayout)
{
    if (newLayout == imageLayout)
    {
        return nullptr;
    }

    rhi::ImageLayout oldLayout = imageLayout;
    imageLayout = newLayout;

    return new Transition(oldLayout, newLayout, mHandle, getWholeImageSubresourceRange());
}

VkImageSubresourceRange Image::getWholeImageSubresourceRange()
{
    VkImageSubresourceRange wholeImageSubresourceRange = {format.aspects, 0, mipLevels, 0, layers};

    return wholeImageSubresourceRange;
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
    ASSERT(valid());

    VkComponentMapping components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,
                                     VK_COMPONENT_SWIZZLE_A};

    VkImageSubresourceRange subresourceRange = {format.aspects, 0, mipLevels, 0, layers};

    view = new ImageView();
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

    view = new ImageView();
    return view->init(context, mHandle, format, components, subresourceRange, viewType);
}

VkFormat Image::getFormat()
{
    ASSERT(valid());
    return format.format;
}

VkImageView Image::getView()
{
    ASSERT(valid() && view->valid());
    return view->getHandle();
}

uint32_t Image::getSamples()
{
    ASSERT(valid());
    return samples;
}
} // namespace vk