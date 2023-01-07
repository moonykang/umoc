#pragma once

#include "vulkan/core.h"

namespace vk
{
class CommandBuffer final : public WrappedObject<CommandBuffer, VkCommandBuffer>
{
};
} // namespace vk