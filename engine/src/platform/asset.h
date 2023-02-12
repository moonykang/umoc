#pragma once

#include "common/memorybuffer.h"
#include <string>

namespace platform
{
class Asset
{
  public:
    Asset() = default;

    virtual ~Asset() = default;

    static Asset* createPlatformAsset();

  public:
    virtual Result loadShader(std::string path, util::MemoryBuffer& buffer) = 0;
};
} // namespace platform