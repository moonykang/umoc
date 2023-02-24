#pragma once

#include "util.h"
#include "vulkan/core.h"
#include <queue>

namespace vk
{
class Context;
class CommandBuffer;
class Queue;

const uint32_t kMaxInFlightCommandBuffers = 5;

class CommandBufferAndGarbage
{
  public:
    CommandBufferAndGarbage()
    {
    }

    CommandBufferAndGarbage(CommandBuffer* commandBuffer) : garbageList(), commandBuffer(commandBuffer)
    {
    }

    CommandBufferAndGarbage(GarbageList&& garbageList, CommandBuffer* commandBuffer)
        : garbageList(std::move(garbageList)), commandBuffer(commandBuffer)
    {
    }

    CommandBufferAndGarbage(CommandBufferAndGarbage&& other)
        : garbageList(std::move(other.garbageList)), commandBuffer(other.commandBuffer)
    {
        other.commandBuffer = nullptr;
    }

    CommandBuffer* getCommandBuffer()
    {
        return commandBuffer;
    }

    GarbageList& getGarbage()
    {
        return garbageList;
    }

  private:
    GarbageList garbageList;
    CommandBuffer* commandBuffer;
};

class CommandPool final : public WrappedObject<CommandPool, VkCommandPool>
{
  public:
    CommandPool();

    Result init(VkDevice device, uint32_t queueFamilyIndex);

    void terminate(Context* context);

    Result allocateCommandBuffers(VkDevice device);

    void prepareActiveCommandBuffer(VkDevice device);

    void prepareUploadCommandBuffer(VkDevice device);

    CommandBuffer* getActiveCommandBuffer(const Context* context);

    CommandBuffer* getUploadCommandBuffer(const Context* context);

    std::vector<VkCommandBuffer> prepareSubmit();

    Result submitActiveCommandBuffer(Context* context, Queue* queue, std::vector<VkSemaphore>& waitSemaphores,
                                     std::vector<VkSemaphore>& signalSemaphores, GarbageList&& currentGarbage);

    Result submitUploadCommandBuffer(Context* context, Queue* queue);

    void resetCommandBuffers(Context* context, bool bIdle = false);

  private:
    const uint32_t DEFAULT_NUM_COMMAND_BUFFER = 10;

    CommandBuffer* uploadCommandBuffer;
    CommandBuffer* activeCommandBuffer;
    std::queue<CommandBufferAndGarbage> submitCommandBuffers;
    std::queue<CommandBuffer*> readyCommandBuffers;
    std::queue<std::pair<int, GarbageList>> garbageListQueue;
};
} // namespace vk