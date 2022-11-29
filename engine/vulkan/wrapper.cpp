#include "vulkan/wrapper.h"

namespace vk
{
namespace handle
{
void Instance::destroy()
{
    if (valid())
    {
        vkDestroyInstance(mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

VkResult Instance::init(const VkInstanceCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateInstance(&createInfo, nullptr, &mHandle);
}
} // namespace handle
} // namespace vk