#include "image.h"
#include "context.h"
#include "descriptor.h"
#include "platform/asset.h"
#include "platform/defines.h"

namespace rhi
{
Image::Image(std::string name, DescriptorType descriptorType)
    : Descriptor(descriptorType), name(name), format(Format::NONE), imageLayout(ImageLayout::Undefined),
      imageUsage(ImageUsage::NONE), imageType(ImageType::IMAGE_2D)
{
}

Extent3D Image::getExtent()
{
    return extent;
}

Texture::Texture(std::string name) : image(nullptr), name(name)
{
}

Result Texture::init(Context* context, std::string path, platform::ImageLoader imageLoader)
{
    Extent2D extent;
    uint32_t numLevels;
    uint32_t numLayers;
    util::MemoryBuffer buffer;
    std::vector<std::vector<size_t>> offsets;
    Format format;

    try(context->getAssetManager()->loadImage(imageLoader, path, format, extent, numLevels, numLayers, offsets,
                                              buffer));

    image = context->allocateImage(name, DescriptorType::Combined_Image_Sampler);

    ImageType imageType = ImageType::IMAGE_2D;
    if (numLayers == 6)
    {
        imageType = ImageType::IMAGE_CUBE;
    }

    // TODO: IMAGE_2D
    try(image->init(context, format, imageType, ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST,
                    MemoryProperty::DEVICE_LOCAL, numLevels, numLayers, 1, {extent.width, extent.height, 1}));

    try(image->update(context, buffer.size(), buffer.data(), offsets));

    return Result::Continue;
}

Result Texture::init(Context* context, Format format, Extent3D extent, uint32_t mipLevels, uint32_t layers,
                     ImageUsageFlags imageUsageFlags)
{
    image = context->allocateImage(name, rhi::DescriptorType::Combined_Image_Sampler);

    ImageType iamgeType = ImageType::IMAGE_2D;

    if (layers == 6)
    {
        iamgeType = ImageType::IMAGE_CUBE;
    }
    try(image->init(context, format, iamgeType, ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST | imageUsageFlags,
                    MemoryProperty::DEVICE_LOCAL, mipLevels, layers, 1, {extent.width, extent.height, extent.depth}));

    return Result::Continue;
}

Result Texture::init(Context* context, Format format, Extent3D extent, ImageUsageFlags imageUsageFlags, size_t size,
                     void* data)
{
    image = context->allocateImage(name, DescriptorType::Combined_Image_Sampler);

    ImageType imageType = ImageType::IMAGE_2D;

    std::vector<std::vector<size_t>> offsets = {{0}};

    try(image->init(context, format, imageType, ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST,
                    MemoryProperty::DEVICE_LOCAL, 1, 1, 1, {extent.width, extent.height, 1}));

    try(image->update(context, size, data, offsets));

    return Result::Continue;
}

void Texture::terminate(Context* context)
{
    TERMINATE(image, context);
}

ImageDescriptor* Texture::getImageDescriptor()
{
    return new ImageDescriptor(rhi::DescriptorType::Combined_Image_Sampler, image);
}

Image* Texture::getImage()
{
    return image;
}
} // namespace rhi