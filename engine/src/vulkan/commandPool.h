#pragma once

#include "vulkan/core.h"
#include <queue>

namespace vk
{
class CommandBuffer;
class Queue;

const uint32_t kMaxInFlightCommandBuffers = 5;

class CommandPool final : public WrappedObject<CommandPool, VkCommandPool>
{
  public:
    CommandPool();

    Result init(VkDevice device, uint32_t queueFamilyIndex);

    void terminate(VkDevice device);

    Result allocateCommandBuffers(VkDevice device);

    void prepareActiveCommandBuffer(VkDevice device);

    void prepareUploadCommandBuffer(VkDevice device);

    CommandBuffer* getActiveCommandBuffer(VkDevice device);

    CommandBuffer* getUploadCommandBuffer(VkDevice device);

    std::vector<VkCommandBuffer> prepareSubmit();

    Result submitActiveCommandBuffer(VkDevice device, Queue* queue, std::vector<VkSemaphore>* waitSemaphores = nullptr,
                                     std::vector<VkSemaphore>* signalSemaphores = nullptr);

    Result submitUploadCommandBuffer(VkDevice device, Queue* queue);

    void resetCommandBuffers(VkDevice device, bool bIdle = false);

  private:
    const uint32_t DEFAULT_NUM_COMMAND_BUFFER = 10;

    CommandBuffer* uploadCommandBuffer;
    CommandBuffer* activeCommandBuffer;
    std::queue<CommandBuffer*> submitCommandBuffers;
    std::queue<CommandBuffer*> readyCommandBuffers;
};
} // namespace vk