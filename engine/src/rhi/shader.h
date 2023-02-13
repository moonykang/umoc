#pragma once
#include "common/memorybuffer.h"
#include "defines.h"
#include <string>
#include <unordered_map>

namespace rhi
{
class Context;
class ShaderBase
{
  public:
    ShaderBase(std::string name, ShaderStage shaderStage) : name(name), shaderStage(shaderStage), loaded(false)
    {
    }

    virtual ~ShaderBase() = default;

    inline size_t getHash()
    {
        return hash;
    }

    Result loadShader(Context* context);

    size_t size()
    {
        ASSERT(loaded);
        return code.size();
    }

    void* data()
    {
        ASSERT(loaded);
        return code.data();
    }

    ShaderStage getShaderStage()
    {
        return shaderStage;
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
    bool loaded;
    ShaderStage shaderStage;
};

class VertexShaderBase : public ShaderBase
{
  public:
    VertexShaderBase(std::string name, VertexChannelFlags vertexChannelFlags)
        : ShaderBase(name, ShaderStage::Vertex), vertexChannelFlags(vertexChannelFlags)
    {
    }

    VertexChannelFlags getVertexChannelFlags()
    {
        return vertexChannelFlags;
    }

  protected:
    VertexChannelFlags vertexChannelFlags;
};

class PixelShaderBase : public ShaderBase
{
  public:
    PixelShaderBase(std::string name) : ShaderBase(name, ShaderStage::Pixel)
    {
    }
};
} // namespace rhi