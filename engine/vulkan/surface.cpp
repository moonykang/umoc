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

VkSurfaceCapabilitiesKHR Surface::getSurfaceCapabilities(VkPhysicalDevice physicalDevice)
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};

    vk_call(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, mHandle, &surfaceCapabilities));

    return std::move(surfaceCapabilities);
}

std::vector<VkPresentModeKHR> Surface::getSurfacePresentModes(VkPhysicalDevice physicalDevice)
{
    // Get available present modes
    uint32_t presentModeCount;
    vk_call(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, mHandle, &presentModeCount, NULL));
    assert(presentModeCount > 0);

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vk_call(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, mHandle, &presentModeCount, presentModes.data()));

    return std::move(presentModes);
}

std::vector<VkSurfaceFormatKHR> Surface::getSurfaceFormats(VkPhysicalDevice physicalDevice)
{
    // Get list of supported surface formats
    uint32_t formatCount;
    vk_call(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, mHandle, &formatCount, NULL));
    assert(formatCount > 0);

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vk_call(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, mHandle, &formatCount, surfaceFormats.data()));

    return std::move(surfaceFormats);
}
} // namespace vk