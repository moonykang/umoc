#pragma once
#include "common/memorybuffer.h"
#include "defines.h"
#include "resources.h"
#include <mutex>
#include <string>
#include <unordered_map>

namespace rhi
{
class Context;
class DescriptorSet;

class ShaderBase
{
  public:
    ShaderBase(std::string name, ShaderStageFlags shaderStage) : name(name), shaderStage(shaderStage), loaded(false)
    {
    }

    virtual ~ShaderBase() = default;

    inline size_t getHash()
    {
        return hash;
    }

    Result init(Context* context);

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

    ShaderStageFlags getShaderStage()
    {
        return shaderStage;
    }

  protected:
    std::string name;
    /*
    name
    attribute layout (pos / normal / uv / color / ...)
     */

    size_t hash;
    util::MemoryBuffer code;
    bool loaded;
    ShaderStageFlags shaderStage;
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

class ShaderParameters
{
  public:
    ShaderParameters() : vertexShader(nullptr), pixelShader(nullptr)
    {
    }

    virtual ~ShaderParameters() = default;

    virtual std::vector<DescriptorSet*> getDescriptorSets()
    {
        return std::vector<DescriptorSet*>();
    }

  public:
    VertexShaderBase* vertexShader;
    PixelShaderBase* pixelShader;
};
} // namespace rhi