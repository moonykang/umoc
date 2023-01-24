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