#pragma once

#include "vulkan/core.h"

namespace vk
{
class CommandPool final : public WrappedObject<CommandPool, VkCommandPool>
{
  public:
    CommandPool();

    Result init(VkDevice device, uint32_t queueFamilyIndex);

  private:
};
} // namespace vk