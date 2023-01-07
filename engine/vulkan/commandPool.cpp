#include "commandPool.h"

namespace vk
{
CommandPool::CommandPool()
{
}

Result CommandPool::init(VkDevice device, uint32_t queueFamilyIndex)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    vk_try(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &mHandle));

    return Result::Continue;
}
} // namespace vk