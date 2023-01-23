#pragma once

#include "vulkan/core.h"

namespace vk
{
class Fence final : public WrappedObject<Fence, VkFence>
{
  public:
    Fence() = default;

    void terminate(VkDevice device);

    using WrappedObject::operator=;

    VkResult init(VkDevice device, const VkFenceCreateInfo& createInfo);

    VkResult reset(VkDevice device);

    VkResult getStatus(VkDevice device) const;

    VkResult wait(VkDevice device, uint64_t timeout) const;
};

class Semaphore final : public WrappedObject<Semaphore, VkSemaphore>
{
  public:
    Semaphore() = default;

    void terminate(VkDevice device);

    VkResult init(VkDevice device);

    VkResult init(VkDevice device, const VkSemaphoreCreateInfo& createInfo);
};
} // namespace vk