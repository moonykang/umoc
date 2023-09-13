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
rhi::Image* Context::allocateImage(std::string name, rhi::DescriptorType descriptorType)
{
    return new Image(name, descriptorType);
}

Result Context::addTransition(rhi::Image* rhiImage, rhi::ImageLayout layout)
{
    Image* image = reinterpret_cast<Image*>(rhiImage);

    CommandBuffer* commandBuffer = getActiveCommandBuffer();

    commandBuffer->addTransition(image->updateImageLayoutAndBarrier(layout));

    return Result::Continue;
}

Result Context::copyImage(rhi::Image* rhiSrcImage, rhi::ImageSubResource srcRange, rhi::Image* rhiDstImage,
                          rhi::ImageSubResource dstRange, rhi::Extent3D extent)
{
    CommandBuffer* commandBuffer = getActiveCommandBuffer();

    Image* srcImage = reinterpret_cast<Image*>(rhiSrcImage);
    commandBuffer->addTransition(srcImage->updateImageLayoutAndBarrier(rhi::ImageLayout::TransferSrc));
    Image* dstImage = reinterpret_cast<Image*>(rhiDstImage);
    commandBuffer->addTransition(dstImage->updateImageLayoutAndBarrier(rhi::ImageLayout::TransferDst));
    commandBuffer->flushTransitions();

    VkImageCopy copyRegion = {};

    copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.srcSubresource.baseArrayLayer = srcRange.baseArrayLayer;
    copyRegion.srcSubresource.mipLevel = srcRange.baseMipLevel;
    copyRegion.srcSubresource.layerCount = srcRange.layerCount;
    copyRegion.srcOffset = {0, 0, 0}; // todo

    copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.dstSubresource.baseArrayLayer = dstRange.baseArrayLayer;
    copyRegion.dstSubresource.mipLevel = dstRange.baseMipLevel;
    copyRegion.dstSubresource.layerCount = dstRange.layerCount;
    copyRegion.dstOffset = {0, 0, 0};

    copyRegion.extent.width = extent.width;
    copyRegion.extent.height = extent.height;
    copyRegion.extent.depth = extent.depth;

    commandBuffer->copyImage(srcImage->getHandle(), dstImage->getHandle(), copyRegion);

    try(submitActiveCommandBuffer());
    return Result::Continue;
}

Result Context::readBackImage(rhi::Image* rhiSrcImage, rhi::ImageSubResource srcRange, void* data)
{
    CommandBuffer* commandBuffer = getActiveCommandBuffer();

    Image* srcImage = reinterpret_cast<Image*>(rhiSrcImage);
    commandBuffer->addTransition(srcImage->updateImageLayoutAndBarrier(rhi::ImageLayout::TransferSrc));
    commandBuffer->flushTransitions();

    const auto& extent = srcImage->getExtent();
    uint32_t formatSize = 4; // TODO
    size_t size = formatSize * extent.width * extent.height;

    RealBuffer* stagingBuffer = new RealBuffer();

    try(stagingBuffer->init(this, rhi::BufferUsage::TRANSFER_DST,
                            rhi::MemoryProperty::HOST_COHERENT | rhi::MemoryProperty::HOST_VISIBLE, size));
    try(stagingBuffer->map(this, 0, size, data));

    VkBufferImageCopy copyRegion = {};
    copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.imageSubresource.baseArrayLayer = srcRange.baseArrayLayer;
    copyRegion.imageSubresource.mipLevel = srcRange.baseMipLevel;
    copyRegion.imageSubresource.layerCount = srcRange.layerCount;
    copyRegion.imageOffset = {0, 0, 0};
    copyRegion.imageExtent = {extent.width, extent.height, extent.depth};

    commandBuffer->copyImageToBuffer(srcImage->getHandle(), stagingBuffer->getHandle(), copyRegion);

    try(submitActiveCommandBuffer());

    try(waitIdle());

    try(stagingBuffer->rmap(this, 0, size, data));

    TERMINATE(stagingBuffer, this);

    return Result::Continue;
}

Result ImageView::init(Context* context, VkImage image, Format format, VkComponentMapping components,
                       VkImageSubresourceRange subresourceRange, VkImageViewType viewType)
{
    Device* device = context->getDevice();

    this->viewType = viewType;

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

Image::Image(std::string name, rhi::DescriptorType descriptorType)
    : rhi::Image(name, descriptorType), deviceMemory(nullptr), mipLevels(0), layers(0), samples(1), view(nullptr),
      sampler(nullptr)
{
}

// Regular Image
Result Image::init(rhi::Context* rhiContext, rhi::Format format, rhi::ImageType imageType,
                   rhi::ImageUsageFlags imageUsage, rhi::MemoryPropertyFlags memoryProperty, uint32_t mipLevels,
                   uint32_t layers, uint32_t samples, rhi::Extent3D extent, const rhi::SamplerInfo& samplerInfo)
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

    VkComponentMapping components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,
                                     VK_COMPONENT_SWIZZLE_A};

    VkImageSubresourceRange subresourceRange = {kFormatMap[format].aspects, 0, mipLevels, 0, layers};

    try(initView(context, components, subresourceRange));

    sampler = new Sampler();
    try(sampler->init(context, convertToVkFilter(samplerInfo.magFilter), convertToVkFilter(samplerInfo.minFilter),
                      convertToVkSamplerAddressMode(samplerInfo.u), convertToVkSamplerAddressMode(samplerInfo.v),
                      convertToVkSamplerAddressMode(samplerInfo.w), samplerInfo.anisotropyEnable,
                      samplerInfo.maxAnisotropy, convertToSamplerMipmapMode(samplerInfo.mipmapMode),
                      samplerInfo.mipLodBias, samplerInfo.minLod, samplerInfo.maxLod,
                      convertToBorderColor(samplerInfo.borderColor)));

    imageInfo.sampler = sampler->getHandle();
    imageInfo.imageView = view->getHandle();
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // TODO

    try(context->debugMarkerSetObjectName((uint64_t)mHandle, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT, name.c_str()));

    LOGD("Init image %s %p layout %s", name.c_str(), mHandle, getImageLayoutName(imageLayout).c_str());

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
    VkComponentMapping components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,
                                     VK_COMPONENT_SWIZZLE_A};

    VkImageSubresourceRange subresourceRange = {kFormatMap[format].aspects, 0, mipLevels, 0, layers};

    try(initView(context, components, subresourceRange));

    sampler = new Sampler();
    try(sampler->init(context, VK_FILTER_NEAREST, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT,
                      VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, false, 0.f,
                      VK_SAMPLER_MIPMAP_MODE_NEAREST, 0.f, 0.f, 0.f, VK_BORDER_COLOR_INT_OPAQUE_BLACK));

    imageInfo.sampler = sampler->getHandle();
    imageInfo.imageView = view->getHandle();
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // TODO

    try(context->debugMarkerSetObjectName((uint64_t)mHandle, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT, name.c_str()));

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

Result Image::update(rhi::Context* rhiContext, size_t size, void* data, std::vector<std::vector<size_t>>& offsets)
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

        ASSERT(offsets.size() == layers);
        for (uint32_t layer = 0; layer < layers; layer++)
        {
            ASSERT(offsets[layer].size() == mipLevels);
            for (uint32_t level = 0; level < mipLevels; level++)
            {
                size_t offset = offsets[layer][level];

                VkExtent3D copyExtent;
                copyExtent.width = extent.width >> level;
                copyExtent.height = extent.height >> level;
                copyExtent.depth = extent.depth;

                try(copy(context, stagingBuffer, copyExtent, level, layer, offset));
            }
        }
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
    //    LOGD("Image %s %p, %s -> %s", name.c_str(), mHandle, getImageLayoutName(imageLayout).c_str(),
    //         getImageLayoutName(newLayout).c_str());

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

size_t Image::getSize(rhi::ImageSubResource subResource)
{
    return 4 * extent.width * extent.height; // TODO
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

    if (imageType == rhi::ImageType::IMAGE_CUBE)
    {
        imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }

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

    switch (imageType)
    {
    case rhi::ImageType::IMAGE_1D:
        viewType = layers > 1 ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
        break;
    case rhi::ImageType::IMAGE_2D:
        viewType = layers > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
        break;
    case rhi::ImageType::IMAGE_3D:
        viewType = VK_IMAGE_VIEW_TYPE_3D;
        break;
    case rhi::ImageType::IMAGE_CUBE:
        viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        break;
    default:
        UNREACHABLE();
    }

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

VkSampler Image::getSampler()
{
    ASSERT(valid() && sampler->valid());
    return sampler->getHandle();
}

uint32_t Image::getSamples()
{
    ASSERT(valid());
    return samples;
}

Result Sampler::init(Context* context, VkFilter magFilter, VkFilter minFilter, VkSamplerAddressMode addressModeU,
                     VkSamplerAddressMode addressModeV, VkSamplerAddressMode addressModeW, bool anisotropyEnable,
                     float maxAnisotropy, VkSamplerMipmapMode mipmapMode, float mipLodBias, float minLod, float maxLod,
                     VkBorderColor borderColor)
{
    VkSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.pNext = nullptr;
    samplerCreateInfo.magFilter = magFilter;
    samplerCreateInfo.minFilter = minFilter;
    samplerCreateInfo.addressModeU = addressModeU;
    samplerCreateInfo.addressModeV = addressModeV;
    samplerCreateInfo.addressModeW = addressModeW;
    samplerCreateInfo.anisotropyEnable = anisotropyEnable;
    samplerCreateInfo.maxAnisotropy = maxAnisotropy;
    samplerCreateInfo.mipmapMode = mipmapMode;
    samplerCreateInfo.mipLodBias = mipLodBias;
    samplerCreateInfo.minLod = minLod;
    samplerCreateInfo.maxLod = maxLod;
    samplerCreateInfo.borderColor = borderColor;
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
