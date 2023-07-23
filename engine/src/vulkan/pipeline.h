#pragma once

#include "rhi/resources.h"
#include "rhi/shader.h"
#include "vulkan/core.h"
#include <unordered_map>

namespace vk
{
class CommandBuffer;

class Shader : public WrappedObject<Shader, VkShaderModule>
{
  public:
    Shader();

    virtual ~Shader() = default;

    Result init(Context* context, rhi::ShaderBase* shaderBase);

    void terminate(VkDevice device);

    VkPipelineShaderStageCreateInfo getPipelineShaderStageCreateInfo()
    {
        return pipelineShaderStageCreateInfo;
    }

  private:
    VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo;
};

class VertexShader : public rhi::VertexShaderBase, public Shader
{
  public:
    VertexShader(rhi::ResourceID id, std::string name, rhi::VertexChannelFlags vertexChannelFlags);

    VertexShader(rhi::ResourceID id, std::string name, std::vector<rhi::VertexAttribute>& vertexAttribute,
                 uint32_t stride);

    Result initRHI(rhi::Context* context) override;

    void terminateRHI(rhi::Context* context) override;

    void generateInputAttributeDescriptions(rhi::VertexChannelFlags vertexChannelFlags);

    void generateInputAttributeDescriptions(std::vector<rhi::VertexAttribute>& vertexAttribute);

    std::vector<VkVertexInputAttributeDescription>& getVertexInputAttributes();

    uint32_t getStride();

  private:
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
    uint32_t stride;
};

class FragmentShader : public rhi::PixelShaderBase, public Shader
{
  public:
    FragmentShader(rhi::ResourceID id, std::string name);

    Result initRHI(rhi::Context* context) override;

    void terminateRHI(rhi::Context* context) override;
};

class ComputeShader : public rhi::ComputeShaderBase, public Shader
{
  public:
    ComputeShader(rhi::ResourceID id, std::string name);

    Result initRHI(rhi::Context* context) override;

    void terminateRHI(rhi::Context* context) override;
};

class PipelineLayout : public WrappedObject<PipelineLayout, VkPipelineLayout>
{
  public:
    Result init(Context* context, std::vector<VkDescriptorSetLayout>& descriptorSetLayouts,
                std::vector<rhi::PushConstant>& pushConstants);

    void terminate(VkDevice device);

  private:
};

class Pipeline : public WrappedObject<Pipeline, VkPipeline>
{
  public:
    Pipeline(VkPipelineBindPoint pipelineBindPoint);

    virtual ~Pipeline() = default;

    Result init(Context* context);

    void bind(CommandBuffer* commandBuffer);

    void terminate(VkDevice device);

    VkResult createGraphics(VkDevice device, const VkGraphicsPipelineCreateInfo& createInfo,
                            const VkPipelineCache& pipelineCache);

    VkResult createCompute(VkDevice device, const VkComputePipelineCreateInfo& createInfo,
                           const VkPipelineCache& pipelineCache);

    PipelineLayout* getLayout()
    {
        return layout;
    }

    VkPipelineBindPoint getPipelineBindPoint()
    {
        return pipelineBindPoint;
    }

  private:
    PipelineLayout* layout;

    VkPipelineBindPoint pipelineBindPoint;
};

class PipelineCache final : public WrappedObject<PipelineCache, VkPipelineCache>
{
  public:
    PipelineCache() = default;

    void terminate(VkDevice device);

    VkResult init(VkDevice device, const VkPipelineCacheCreateInfo& createInfo);

    VkResult getCacheData(VkDevice device, size_t* cacheSize, void* cacheData);

    VkResult merge(VkDevice device, uint32_t srcCacheCount, const VkPipelineCache* srcCaches);
};

class PipelineMap
{
  public:
    PipelineMap();

    ~PipelineMap() = default;

    Result init(Context* context);

    void terminate(VkDevice device);

    Pipeline* getPipeline(Context* context, rhi::GraphicsPipelineState& gfxPipelineState);

    Pipeline* getPipeline(Context* context, rhi::ComputePipelineState& pipelineState);

  private:
    std::unordered_map<size_t, Pipeline*> pipelineMap;

    std::unordered_map<size_t, Pipeline*> computePipelineMap;
    PipelineCache* pipelineCache;
};
} // namespace vk