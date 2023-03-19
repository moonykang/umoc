#pragma once

#include "common/util.h"
#include "platform/defines.h"
#include "resources.h"
#include <string>

namespace rhi
{
class Context;
class Image : public Descriptor
{
  public:
    Image(DescriptorType descriptorType);

    virtual ~Image() = default;

    virtual Result init(Context* context, Format format, ImageType imageType, ImageUsageFlags imageUsage,
                        MemoryPropertyFlags memoryProperty, uint32_t mipLevels, uint32_t layers, uint32_t samples,
                        Extent3D extent) = 0;

    virtual Result update(Context* context, size_t size, void* data, std::vector<size_t>& mipOffsets) = 0;

    virtual void terminate(Context* context) = 0;

  protected:
    Format format;
    ImageLayout imageLayout;
    ImageUsageFlags imageUsage;
    ImageType imageType;
    Extent3D extent;
};

class ImageDescriptor;
class Texture
{
  public:
    Texture();

    virtual ~Texture() = default;

    Result init(Context* context, std::string path, platform::ImageLoader imageLoader);

    void terminate(Context* context);

    ImageDescriptor* getImageDescriptor();

  private:
    Image* image;
};
} // namespace rhi