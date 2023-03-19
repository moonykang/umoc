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
    rhi::Extent3D extent;
    util::MemoryBuffer buffer;
    std::vector<size_t> mipOffsets;
    rhi::Format format;

    try(context->getAssetManager()->loadImage(imageLoader, path, format, extent, mipOffsets, buffer));

    image = context->allocateImage(rhi::DescriptorType::Combined_Image_Sampler);

    try(image->init(context, format, rhi::ImageType::IMAGE_2D, rhi::ImageUsage::SAMPLED | rhi::ImageUsage::TRANSFER_DST,
                    rhi::MemoryProperty::DEVICE_LOCAL, extent.depth, 1, 1, {extent.width, extent.height, 1}));

    try(image->update(context, buffer.size(), buffer.data(), mipOffsets));

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
} // namespace rhi