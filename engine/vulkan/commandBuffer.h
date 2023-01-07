#pragma once

#include "vulkan/core.h"

namespace vk
{
class Fence;

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

  private:
    VkResult allocate(VkDevice device, const VkCommandBufferAllocateInfo& allocateInfo);

    VkResult reset();

  private:
    Fence* fence;
};
} // namespace vk