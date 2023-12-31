#pragma once

#include "defines.h"
#include "platform/context.h"
#include "resources.h"
#include <unordered_map>

namespace rhi
{
class Image;
class VertexBuffer;
class IndexBuffer;
class UniformBuffer;
class StorageBuffer;
class VertexScratchBuffer;
class IndexScratchBuffer;
class UniformScratchBuffer;
class StorageScratchBuffer;
class VertexShaderBase;
class PixelShaderBase;
class ComputeShaderBase;

class Buffer;
class DescriptorSetLayout;
class DescriptorSet;

class Context : public platform::Context
{
  public:
    static Context* createRHIContext(rhi::List rhi);

  public:
    Context();

    Result initRHI(platform::Window* window) override final;

    virtual Result initRHIImplementation(platform::Window* window) = 0;

    virtual void terminateRHI() override final;

    virtual void terminateRHIImplementation() = 0;

    virtual Result flush() = 0;

    virtual Result present() = 0;

    virtual Result waitIdle() = 0;

    virtual Image* getCurrentSurfaceImage() = 0;

    virtual Extent3D getSurfaceSize() = 0;

    virtual Result beginRenderpass(RenderPassInfo& renderpassInfo) = 0;

    virtual Result endRenderpass() = 0;

    virtual Result createGfxPipeline(GraphicsPipelineState gfxPipelineState) = 0;

    virtual Result createComputePipeline(ComputePipelineState pipelineState) = 0;

    virtual void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;

    virtual void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset,
                             uint32_t firstInstance) = 0;

    virtual void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

    virtual Result addTransition(Image* image, ImageLayout layout) = 0;

    // Use ImageLayout for specifying pipeline stages
    virtual Result addTransition(Buffer* buffer, size_t offset, size_t size, ImageLayout src, ImageLayout dst) = 0;

    virtual Result flushTransition() = 0;

    virtual void pushConstant(ShaderStageFlags shaderStage, size_t size, void* data) = 0;

    virtual Result copyImage(Image* srcImage, ImageSubResource srcRange, Image* dstImage, ImageSubResource dstRagne,
                             Extent3D extent) = 0;

    virtual Result readBackImage(Image* srcImage, ImageSubResource srcRange, void* data) = 0;

    virtual Result viewport(Extent2D extent) = 0;

  public:
    VertexBuffer* allocateVertexBuffer(size_t size, void* data);

    IndexBuffer* allocateIndexBuffer(size_t size, void* data);

    UniformBuffer* allocateUniformBuffer(size_t size, void* data);

    StorageBuffer* allocateStorageBuffer(size_t size, void* data);

    VertexShaderBase* allocateVertexShader(std::string name, VertexChannelFlags vertexChannelFlags);

    VertexShaderBase* allocateVertexShader(std::string name, std::vector<VertexAttribute>& vertexAttribute,
                                           uint32_t stride);

    PixelShaderBase* allocatePixelShader(std::string name);

    ComputeShaderBase* allocateComputeShader(std::string name);
    // factory
  public:
    virtual Buffer* allocateBuffer(BufferUsageFlags bufferUsage, MemoryPropertyFlags memoryProperty, size_t size) = 0;

    virtual Image* allocateImage(std::string name, DescriptorType descriptorType) = 0;

    virtual DescriptorSet* allocateDescriptorSet() = 0;

    virtual VertexShaderBase* createVertexShader(ResourceID id, std::string name,
                                                 VertexChannelFlags vertexChannelFlags) = 0;

    virtual VertexShaderBase* createVertexShader(ResourceID id, std::string name,
                                                 std::vector<VertexAttribute>& vertexAttribute, uint32_t stride) = 0;

    virtual PixelShaderBase* createPixelShader(ResourceID id, std::string name) = 0;

    virtual ComputeShaderBase* createComputeShader(ResourceID id, std::string name) = 0;

  private:
    VertexScratchBuffer* vertexScratchBuffer;
    IndexScratchBuffer* indexScratchBuffer;
    UniformScratchBuffer* uniformScratchBuffer;
    StorageScratchBuffer* storageScratchBuffer;
    std::unordered_map<ResourceID, ShaderBase*> shaderMap;
};
} // namespace rhi