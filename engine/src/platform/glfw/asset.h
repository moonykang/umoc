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

    Result loadShader(std::string path, util::MemoryBuffer& buffer) override;
};
} // namespace glfw
} // namespace platform