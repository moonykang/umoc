#pragma once
#include "common/memorybuffer.h"
#include "defines.h"
#include <string>

namespace rhi
{
class ShaderBase
{
  public:
    ShaderBase(std::string name) : name(name)
    {
    }

    virtual ~ShaderBase() = default;

    inline size_t getHash()
    {
        return hash;
    }

  protected:
    std::string name;
    /*
    name
    attribute layout (pos / normal / uv / color / ...)
    ub layout
    out layout
     */
    size_t hash;
    util::MemoryBuffer code;
};

class VertexShaderBase : public ShaderBase
{
  public:
    VertexShaderBase(std::string name, VertexChannelFlags vertexChannelFlags)
        : ShaderBase(name), vertexChannelFlags(vertexChannelFlags)
    {
    }

  protected:
    VertexChannelFlags vertexChannelFlags;
};

class PixelShaderBase : public ShaderBase
{
};
} // namespace rhi