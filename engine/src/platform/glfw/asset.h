#pragma once

#include "platform/asset.h"

namespace platform
{
namespace glfw
{
class Asset : public platform::Asset
{
  public:
    Asset();

    std::string getAssetPath() override;

    Result loadShader(std::string path, util::MemoryBuffer& buffer) override;

    Result loadImage(ImageLoader loader, std::string path, rhi::Format& format, rhi::Extent2D& extent,
                     uint32_t& numLevels, uint32_t& numLayers, std::vector<std::vector<size_t>>& offsets,
                     util::MemoryBuffer& buffer) override;
};
} // namespace glfw
} // namespace platform