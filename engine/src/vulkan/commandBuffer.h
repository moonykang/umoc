#pragma once

#include "vulkan/core.h"

namespace vk
{
class Fence;
class Transition;

class CommandBuffer final : public WrappedObject<CommandBuffer, VkCommandBuffer>
{
  public:
    CommandBuffer();

    ~CommandBuffer() = default;

    Result init(VkDevice device, VkCommandPool commandPool);

    void terminate(VkDevice device, VkCommandPool commandPool);

    Result reset(VkDevice device, const bool bWait);

    VkResult begin();

    VkResult end();

    VkFence getFence();

    void addTransition(Transition* newTransition);

    void flushTransitions();

    /*
    Begin of command functions
    */
  public:
    inline void beginRenderPass(const VkRenderPassBeginInfo& beginInfo, VkSubpassContents subpassContents)
    {
        ASSERT(valid());
        flushTransitions();
        vkCmdBeginRenderPass(mHandle, &beginInfo, subpassContents);
    }

    inline void endRenderPass()
    {
        ASSERT(valid());
        vkCmdEndRenderPass(mHandle);
    }

    inline void pipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                                VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount,
                                const VkMemoryBarrier* memoryBarriers, uint32_t bufferMemoryBarrierCount,
                                const VkBufferMemoryBarrier* bufferMemoryBarriers, uint32_t imageMemoryBarrierCount,
                                const VkImageMemoryBarrier* imageMemoryBarriers)
    {
        ASSERT(valid());
        vkCmdPipelineBarrier(mHandle, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, memoryBarriers,
                             bufferMemoryBarrierCount, bufferMemoryBarriers, imageMemoryBarrierCount,
                             imageMemoryBarriers);
    }

    inline void bindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline)
    {
        ASSERT(valid());
        vkCmdBindPipeline(mHandle, pipelineBindPoint, pipeline);
    }

    inline void setViewport(const VkViewport& viewport)
    {
        ASSERT(valid());
        vkCmdSetViewport(mHandle, 0, 1, &viewport);
    }

    inline void setScissor(const VkRect2D& scissor)
    {
        ASSERT(valid());
        vkCmdSetScissor(mHandle, 0, 1, &scissor);
    }

    inline void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
    {
        ASSERT(valid());
        vkCmdDraw(mHandle, vertexCount, instanceCount, firstVertex, firstInstance);
    }
    /*
    End of command functions
    */
  private:
    VkResult allocate(VkDevice device, const VkCommandBufferAllocateInfo& allocateInfo);

    VkResult reset();

  private:
    Fence* fence;
    Transition* transition;
};
} // namespace vk