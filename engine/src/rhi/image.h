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
    Image(std::string name, DescriptorType descriptorType);

    virtual ~Image() = default;

    virtual Result init(Context* context, Format format, ImageType imageType, ImageUsageFlags imageUsage,
                        MemoryPropertyFlags memoryProperty, uint32_t mipLevels, uint32_t layers, uint32_t samples,
                        Extent3D extent) = 0;

    virtual Result update(Context* context, size_t size, void* data, std::vector<std::vector<size_t>>& offsets) = 0;

    virtual void terminate(Context* context) = 0;

  protected:
    std::string name;
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

    // For texture loading
    Result init(Context* context, std::string name, std::string path, platform::ImageLoader imageLoader);

    // For render targets
    Result init(Context* context, std::string name, Format format, Extent3D extent, uint32_t mipLevels, uint32_t layers,
                ImageUsageFlags imageUsageFlags);

    void terminate(Context* context);

    ImageDescriptor* getImageDescriptor();

    Image* getImage();

  private:
    Image* image;
};
} // namespace rhi