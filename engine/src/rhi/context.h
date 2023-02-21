#pragma once

#include "defines.h"
#include "platform/context.h"
#include "resources.h"
#include <unordered_map>

namespace rhi
{
class Image;
class Renderpass;
class VertexScratchBuffer;
class IndexScratchBuffer;
class Buffer;

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

    virtual Result beginRenderpass(RenderPassInfo& renderpassInfo) = 0;

    virtual Result endRenderpass() = 0;

    virtual Result createGfxPipeline(GraphicsPipelineState gfxPipelineState) = 0;

    virtual void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;

  public:
    VertexScratchBuffer* getVertexScratchBuffer()
    {
        ASSERT(vertexScratchBuffer);
        return vertexScratchBuffer;
    }

    IndexScratchBuffer* getIndexScratchBuffer()
    {
        ASSERT(indexScratchBuffer);
        return indexScratchBuffer;
    }
    // factory
  public:
    virtual Buffer* createBuffer(BufferUsageFlags bufferUsage, MemoryPropertyFlags memoryProperty, size_t size) = 0;

  private:
    VertexScratchBuffer* vertexScratchBuffer;
    IndexScratchBuffer* indexScratchBuffer;
};
} // namespace rhi