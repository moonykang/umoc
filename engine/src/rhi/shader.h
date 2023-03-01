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

class DescriptorSetLayout;
class ShaderContainer
{
  public:
    ShaderContainer();

    virtual ~ShaderContainer() = default;

    Result init(Context* context);

    void terminate(Context* context);

    virtual DescriptorInfoListSet getDescriptorListSet() = 0;

    class ShaderParameters
    {
      public:
        virtual ~ShaderParameters() = default;

      protected:
        static inline void fillDescriptorInfo(DescriptorInfoListSet& descriptorInfoSet, uint32_t set, uint32_t binding,
                                              ShaderStageFlags stage, DescriptorType type)
        {
            ASSERT(descriptorInfoSet.size() >= set);
            descriptorInfoSet[set].push_back({binding, stage, type});
        }

        static inline void fillDescriptor(DescriptorListSet& descriptorInfoSet, uint32_t set, uint32_t binding,
                                          ShaderStageFlags stage, DescriptorType type, Descriptor* descriptor)
        {
            ASSERT(descriptorInfoSet.size() >= set);
            descriptorInfoSet[set].push_back({{binding, stage, type}, descriptor});
        }
    };

  public:
    VertexShaderBase* getVertexShader();

    PixelShaderBase* getPixelShader();

    DescriptorSetLayout* getDescriptorLayout(uint32_t i)
    {
        return descriptorSetLayouts[i];
    }

    std::vector<DescriptorSetLayout*> getDescriptorLayouts()
    {
        return descriptorSetLayouts;
    }

  private:
    bool initialized;
    std::mutex lock;
    size_t hash;

  protected:
    VertexShaderBase* vertexShader;
    PixelShaderBase* pixelShader;
    std::vector<DescriptorSetLayout*> descriptorSetLayouts;
};
} // namespace rhi