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

    ~Shader() = default;

    Result init(Context* context, rhi::ShaderBase* shaderBase);

    void terminate(VkDevice device);

    VkPipelineShaderStageCreateInfo getPipelineShaderStageCreateInfo()
    {
        return pipelineShaderStageCreateInfo;
    }

  private:
    VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo;
};

class ShaderMap
{
  public:
    Shader* getShader(Context* context, rhi::ShaderBase* shader);

    void terminate(VkDevice device);

  private:
    std::unordered_map<size_t, Shader*> shaderMap;
};

class PipelineLayout : public WrappedObject<PipelineLayout, VkPipelineLayout>
{
  public:
    Result init(Context* context);

    void terminate(VkDevice device);

  private:
};

class Pipeline : public WrappedObject<Pipeline, VkPipeline>
{
  public:
    Pipeline();

    ~Pipeline() = default;

    Result init(Context* context);

    void bind(CommandBuffer* commandBuffer);

    void terminate(VkDevice device);

    VkResult createGraphics(VkDevice device, const VkGraphicsPipelineCreateInfo& createInfo,
                            const VkPipelineCache& pipelineCache);

    PipelineLayout* getLayout()
    {
        return layout;
    }

  private:
    PipelineLayout* layout;
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

  private:
    std::unordered_map<size_t, Pipeline*> pipelineMap;
    PipelineCache* pipelineCache;
};
} // namespace vk