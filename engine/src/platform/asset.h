#pragma once

#include "common/memorybuffer.h"
#include "defines.h"
#include "rhi/defines.h"
#include <string>

namespace platform
{
class Asset
{
  public:
    Asset() = default;

    virtual ~Asset() = default;

    static Asset* createPlatformAsset();

    virtual std::string getAssetPath() = 0;

  public:
    virtual Result loadShader(std::string path, util::MemoryBuffer& buffer) = 0;

    virtual Result loadImage(ImageLoader loader, std::string path, rhi::Format& format, rhi::Extent2D& extent,
                             uint32_t& numLevels, uint32_t& numLayers, std::vector<std::vector<size_t>>& offsets,
                             util::MemoryBuffer& buffer) = 0;
};
} // namespace platform