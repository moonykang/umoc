#include "vulkan/surface.h"
#include "vulkan/physical_device.h"

namespace vk
{
Surface::Surface() : surfaceCapabilities()
{
}

void Surface::terminate(VkInstance instance)
{
    if (valid())
    {
        vkDestroySurfaceKHR(instance, mHandle, nullptr);
    }
}

Result Surface::updateSurfaceCapabilities(PhysicalDevice* physicalDevice)
{
    vk_try(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->getHandle(), mHandle, &surfaceCapabilities));

    return Result::Continue;
}
} // namespace vk