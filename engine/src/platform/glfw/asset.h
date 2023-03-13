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

    Result loadImage(ImageLoader loader, std::string path, rhi::Format& format, rhi::Extent3D& extent,
                     std::vector<std::pair<uint32_t, size_t>>& mipOffsets, util::MemoryBuffer& buffer) override;
};
} // namespace glfw
} // namespace platform