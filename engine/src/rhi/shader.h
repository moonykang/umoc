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

class ShaderBase : public Resource
{
  public:
    ShaderBase(std::string name, ShaderStageFlags shaderStage) : name(name), shaderStage(shaderStage), loaded(false)
    {
    }

    virtual ~ShaderBase() = default;

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

    Result generateID() override
    {
        std::lock_guard<std::mutex> local_lock(lock);

        id = util::computeGenericHash(name.data(), name.size() * sizeof(char));

        return Result::Continue;
    }

  protected:
    std::string name;
    /*
    name
    attribute layout (pos / normal / uv / color / ...)
     */

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
    ShaderParameters()
        : vertexShader(nullptr), pixelShader(nullptr), globalDescriptor(nullptr), localDescriptor(nullptr),
          materialDescriptor(nullptr)
    {
    }

    ShaderParameters(ShaderParameters& other)
    {
        this->vertexShader = other.vertexShader;
        this->pixelShader = other.pixelShader;
        this->globalDescriptor = other.globalDescriptor;
        this->localDescriptor = other.localDescriptor;
        this->materialDescriptor = other.materialDescriptor;
    }

    virtual ~ShaderParameters() = default;

    virtual std::vector<DescriptorSet*> getDescriptorSets()
    {
        std::vector<DescriptorSet*> descriptorList;

        if (globalDescriptor)
        {
            descriptorList.push_back(globalDescriptor);
        }

        if (localDescriptor)
        {
            descriptorList.push_back(localDescriptor);
        }

        if (materialDescriptor)
        {
            descriptorList.push_back(materialDescriptor);
        }
        return descriptorList;
    }

  public:
    VertexShaderBase* vertexShader;
    PixelShaderBase* pixelShader;

    rhi::DescriptorSet* globalDescriptor;
    rhi::DescriptorSet* localDescriptor;
    rhi::DescriptorSet* materialDescriptor;
};
} // namespace rhi
