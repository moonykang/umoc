#pragma once

#include "common/util.h"
#include "platform/defines.h"
#include "resources.h"
#include <string>

namespace rhi
{
class Context;

class Sampler
{
  public:
  private:
};

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

    virtual size_t getSize(rhi::ImageSubResource subResource) = 0;

    Extent3D getExtent();

  protected:
    std::string name;
    Format format;
    ImageLayout imageLayout;
    ImageUsageFlags imageUsage;
    ImageType imageType;
    Extent3D extent;
};

class ImageDescriptor;
class Texture : public Resource
{
  public:
    Texture(std::string name);

    virtual ~Texture() = default;

    // For texture loading
    Result init(Context* context, std::string path, platform::ImageLoader imageLoader);

    // For render targets
    Result init(Context* context, Format format, Extent3D extent, uint32_t mipLevels, uint32_t layers,
                ImageUsageFlags imageUsageFlags);

    // For texture loading
    Result init(Context* context, Format format, Extent3D extent, ImageUsageFlags imageUsageFlags, size_t size,
                void* data);

    void terminate(Context* context);

    ImageDescriptor* getImageDescriptor();

    Image* getImage();

    Result generateID() override
    {
        std::lock_guard<std::mutex> local_lock(lock);
        name.reserve(128);
        id = util::computeGenericHash(name.data(), name.size() * sizeof(char));

        return Result::Continue;
    }

  private:
    Image* image;
    std::string name;
};
} // namespace rhi