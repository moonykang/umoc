#include "image.h"
#include "context.h"
#include "descriptor.h"
#include "platform/asset.h"
#include "platform/defines.h"

namespace rhi
{
Image::Image(DescriptorType descriptorType)
    : Descriptor(descriptorType), format(Format::NONE), imageLayout(ImageLayout::Undefined),
      imageUsage(ImageUsage::NONE), imageType(ImageType::IMAGE_2D)
{
}

Texture::Texture() : image(nullptr)
{
}

Result Texture::init(Context* context, std::string path, platform::ImageLoader imageLoader)
{
    Extent3D extent;
    util::MemoryBuffer buffer;
    std::vector<size_t> mipOffsets;
    Format format;

    try(context->getAssetManager()->loadImage(imageLoader, path, format, extent, mipOffsets, buffer));

    image = context->allocateImage(DescriptorType::Combined_Image_Sampler);

    // TODO: IMAGE_2D
    try(image->init(context, format, ImageType::IMAGE_2D, ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST,
                    MemoryProperty::DEVICE_LOCAL, extent.depth, 1, 1, {extent.width, extent.height, 1}));

    try(image->update(context, buffer.size(), buffer.data(), mipOffsets));

    return Result::Continue;
}

Result Texture::init(Context* context, Format format, Extent3D extent, ImageUsageFlags imageUsageFlags)
{
    image = context->allocateImage(rhi::DescriptorType::Combined_Image_Sampler);

    try(image->init(context, format, ImageType::IMAGE_2D,
                    ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST | imageUsageFlags, MemoryProperty::DEVICE_LOCAL, 1,
                    1, 1, {extent.width, extent.height, extent.depth}));

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