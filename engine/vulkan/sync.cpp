#include "vulkan/sync.h"

namespace vk
{
void Fence::terminate(VkDevice device)
{
    if (valid())
    {
        vkDestroyFence(device, mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

VkResult Fence::init(VkDevice device, const VkFenceCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateFence(device, &createInfo, nullptr, &mHandle);
}

VkResult Fence::reset(VkDevice device)
{
    ASSERT(valid());
    return vkResetFences(device, 1, &mHandle);
}

VkResult Fence::getStatus(VkDevice device) const
{
    ASSERT(valid());
    return vkGetFenceStatus(device, mHandle);
}

VkResult Fence::wait(VkDevice device, uint64_t timeout) const
{
    ASSERT(valid());
    return vkWaitForFences(device, 1, &mHandle, true, timeout);
}
} // namespace vk