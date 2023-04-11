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
    ShaderBase(ResourceID id, std::string name, ShaderStageFlags shaderStage)
        : Resource(id), name(name), shaderStage(shaderStage), loaded(false)
    {
    }

    virtual ~ShaderBase() = default;

    Result init(Context* context);

    void terminate(Context* context);

    virtual Result initRHI(Context* context) = 0;

    virtual void terminateRHI(Context* context) = 0;

    size_t size()
    {
        return code.size();
    }

    void* data()
    {
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
    VertexShaderBase(ResourceID id, std::string name, VertexChannelFlags vertexChannelFlags)
        : ShaderBase(id, name, ShaderStage::Vertex), vertexChannelFlags(vertexChannelFlags)
    {
    }

    virtual Result initRHI(Context* context) = 0;

    virtual void terminateRHI(Context* context) = 0;

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
    PixelShaderBase(ResourceID id, std::string name) : ShaderBase(id, name, ShaderStage::Pixel)
    {
    }

    virtual Result initRHI(Context* context) = 0;

    virtual void terminateRHI(Context* context) = 0;
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
