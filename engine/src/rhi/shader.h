#pragma once
#include "common/memorybuffer.h"
#include <string>

namespace rhi
{
class ShaderBase
{
  public:
    ShaderBase(std::string name) : name(name)
    {
    }

    ~ShaderBase() = default;

  private:
    std::string name;
    /*
    name
    attribute layout (pos / normal / uv / color / ...)
    ub layout
    out layout
     */
    size_t hash;
    MemoryBuffer code;
};
} // namespace rhi