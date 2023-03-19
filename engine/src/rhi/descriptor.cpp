#include "descriptor.h"

namespace rhi
{
BufferDescriptor::BufferDescriptor(rhi::DescriptorType type, Buffer* buffer, size_t offset, size_t range)
    : Descriptor(type), buffer(buffer), offset(offset), range(range)
{
}

Buffer* BufferDescriptor::getBuffer()
{
    return buffer;
}

size_t BufferDescriptor::getOffset()
{
    return offset;
}

size_t BufferDescriptor::getRange()
{
    return range;
}

ImageDescriptor::ImageDescriptor(rhi::DescriptorType type, Image* image) : Descriptor(type), image(image)
{
}

Image* ImageDescriptor::getImage()
{
    return image;
}
} // namespace rhi