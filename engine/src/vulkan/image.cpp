#include "image.h"
#include "buffer.h"
#include "commandBuffer.h"
#include "context.h"
#include "device.h"
#include "memory.h"
#include "physicalDevice.h"
#include "transition.h"

namespace vk
{
rhi::Image* Context::allocateImage(rhi::DescriptorType descriptorType)
{
    return new Image(descriptorType);
}

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

Image::Image(rhi::DescriptorType descriptorType)
    : rhi::Image(descriptorType), deviceMemory(nullptr), mipLevels(0), layers(0), samples(1), view(nullptr),
      sampler(nullptr)
{
}

// Regular Image
Result Image::init(rhi::Context* rhiContext, rhi::Format format, rhi::ImageType imageType,
                   rhi::ImageUsageFlags imageUsage, rhi::MemoryPropertyFlags memoryProperty, uint32_t mipLevels,
                   uint32_t layers, uint32_t samples, rhi::Extent3D extent)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);

    this->format = format;
    this->imageType = imageType;
    this->mipLevels = mipLevels;
    this->layers = layers;
    this->samples = samples;
    this->extent = std::move(extent);
    this->imageUsage = imageUsage;

    try(initImage(context));

    const VkMemoryRequirements memoryRequirements = getMemoryRequirements(context->getDevice()->getHandle());

    deviceMemory = new DeviceMemory();
    try(deviceMemory->init(context, memoryRequirements, memoryProperty));

    vk_try(bindMemory(context->getDevice()->getHandle()));

    VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
    // TODO
    /*
    if (extent.depth > 1)
    {
        viewType = VK_IMAGE_VIEW_TYPE_3D;
    }
    else if (layers > 1)
    {
        viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    }*/

    VkComponentMapping components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,
                                     VK_COMPONENT_SWIZZLE_A};

    VkImageSubresourceRange subresourceRange = {kFormatMap[format].aspects, 0, mipLevels, 0, layers};

    try(initView(context, components, subresourceRange));

    sampler = new Sampler();
    try(sampler->init(context));

    imageInfo.sampler = sampler->getHandle();
    imageInfo.imageView = view->getHandle();
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // TODO

    return Result::Continue;
}

// swapchain image
Result Image::init(Context* context, VkImage image, rhi::Format format, rhi::ImageType imageType,
                   rhi::ImageUsageFlags imageUsage, uint32_t mipLevels, uint32_t layers, uint32_t samples,
                   rhi::Extent3D extent)
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

    // TODO
    /*
    if (extent.depth > 1)
    {
        viewType = VK_IMAGE_VIEW_TYPE_3D;
    }
    else if (layers > 1)
    {
        viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    }*/

    VkComponentMapping components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,
                                     VK_COMPONENT_SWIZZLE_A};

    VkImageSubresourceRange subresourceRange = {kFormatMap[format].aspects, 0, mipLevels, 0, layers};

    try(initView(context, components, subresourceRange));

    sampler = new Sampler();
    try(sampler->init(context));

    imageInfo.sampler = sampler->getHandle();
    imageInfo.imageView = view->getHandle();
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // TODO

    return Result::Continue;
}

void Image::terminate(rhi::Context* rhiContext)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);

    if (valid())
    {
        context->addGarbage(HandleType::Image, mHandle);
        mHandle = VK_NULL_HANDLE;
    }

    TERMINATE(sampler, context);
    TERMINATE(view, context);
    TERMINATE(deviceMemory, context);
}

void Image::release(Context* context)
{
    TERMINATE(sampler, context);
    TERMINATE(view, context);
    TERMINATE(deviceMemory, context);
    mHandle = VK_NULL_HANDLE;
}

Result Image::update(rhi::Context* rhiContext, size_t size, void* data)
{
    Context* context = reinterpret_cast<Context*>(rhiContext);

    CommandBuffer* commandBuffer = context->getUploadCommandBuffer();

    RealBuffer* stagingBuffer = new RealBuffer();

    try(stagingBuffer->init(context, rhi::BufferUsage::TRANSFER_SRC,
                            rhi::MemoryProperty::HOST_COHERENT | rhi::MemoryProperty::HOST_VISIBLE, size));
    try(stagingBuffer->map(context, 0, size, data));

    {

        commandBuffer->addTransition(updateImageLayoutAndBarrier(rhi::ImageLayout::TransferDst));
        commandBuffer->flushTransitions();

        uint32_t mipLevel = 0;
        size_t bufferOffset = 0;

        VkExtent3D copyExtent;
        copyExtent.width = extent.width >> mipLevel;
        copyExtent.height = extent.height >> mipLevel;
        copyExtent.depth = extent.depth;

        try(copy(context, stagingBuffer, copyExtent, mipLevel, 0, bufferOffset));

        commandBuffer->addTransition(updateImageLayoutAndBarrier(rhi::ImageLayout::FragmentShaderReadOnly));
    }

    TERMINATE(stagingBuffer, context);
    return Result::Continue;
}

Result Image::copy(Context* context, RealBuffer* srcBuffer, VkExtent3D extent, uint32_t mipLevel, uint32_t layer,
                   size_t offset)
{

    VkBufferImageCopy copyRegion = {};
    copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    // subresourceRange.aspectMask; // TODO
    copyRegion.imageSubresource.mipLevel = mipLevel;
    copyRegion.imageSubresource.baseArrayLayer = layer;
    copyRegion.imageSubresource.layerCount = 1;

    copyRegion.imageOffset = {0, 0, 0};
    copyRegion.imageExtent = extent;
    copyRegion.bufferOffset = offset;

    CommandBuffer* commandBuffer = context->getUploadCommandBuffer();
    commandBuffer->copyBufferToImage(srcBuffer->getHandle(), mHandle, copyRegion);

    return Result::Continue;
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
    VkImageSubresourceRange wholeImageSubresourceRange = {getAspectFlags(), 0, mipLevels, 0, layers};

    return wholeImageSubresourceRange;
}

Result Image::initImage(Context* context)
{
    Device* device = context->getDevice();

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.imageType = convertToVkImageType(imageType);
    imageCreateInfo.format = getFormat();
    imageCreateInfo.mipLevels = mipLevels;
    imageCreateInfo.arrayLayers = layers;
    imageCreateInfo.samples = VkSampleCountFlagBits(samples);
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.extent = {extent.width, extent.height, extent.depth};
    imageCreateInfo.usage = static_cast<VkImageUsageFlags>(imageUsage);

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

VkResult Image::bindMemory(VkDevice device)
{
    ASSERT(valid() && deviceMemory->valid());
    return vkBindImageMemory(device, mHandle, deviceMemory->getHandle(), 0);
}

Result Image::initView(Context* context, VkComponentMapping components, VkImageSubresourceRange subresourceRange)
{
    ASSERT(valid());

    VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;

    // TODO
    /*
    switch (imageType)
    {
    case rhi::ImageType::IMAGE_1D:
        viewType = layers > 0 ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
        break;
    case rhi::ImageType::IMAGE_2D:
        viewType = layers > 0 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
        break;
    case rhi::ImageType::IMAGE_3D:
        viewType = VK_IMAGE_VIEW_TYPE_3D;
        break;
    default:
        UNREACHABLE();
    }*/

    view = new ImageView();
    return view->init(context, mHandle, {getFormat(), getAspectFlags()}, components, subresourceRange, viewType);
}

VkFormat Image::getFormat()
{
    // ASSERT(valid());
    return kFormatMap[format].format;
}

VkImageAspectFlags Image::getAspectFlags()
{
    // ASSERT(valid());
    return kFormatMap[format].aspects;
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

// TODO
Result Sampler::init(Context* context)
{
    VkSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.pNext = nullptr;
    samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
    samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.anisotropyEnable = false;
    samplerCreateInfo.maxAnisotropy = 0.0f;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    samplerCreateInfo.mipLodBias = 0.0f;
    samplerCreateInfo.minLod = 0.0f;
    samplerCreateInfo.maxLod = 0.0f;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerCreateInfo.unnormalizedCoordinates = false;

    vk_try(create(context->getDevice()->getHandle(), samplerCreateInfo));

    return Result::Continue;
}

void Sampler::terminate(Context* context)
{
    if (valid())
    {
        vkDestroySampler(context->getDevice()->getHandle(), mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

VkResult Sampler::create(VkDevice device, const VkSamplerCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateSampler(device, &createInfo, nullptr, &mHandle);
}
} // namespace vk