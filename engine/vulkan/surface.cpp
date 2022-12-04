#include "vulkan/surface.h"

namespace vk
{
Surface::Surface() : surface(VK_NULL_HANDLE)
{
}

VkSurfaceKHR& Surface::getSurface()
{
    return surface;
}

void Surface::terminate(VkInstance instance)
{
    if (surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }
}
} // namespace vk