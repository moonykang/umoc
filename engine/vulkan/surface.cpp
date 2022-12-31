#include "vulkan/surface.h"
#include "vulkan/context.h"
#include "vulkan/physical_device.h"

namespace vk
{
Surface::Surface()
{
}

void Surface::terminate(VkInstance instance)
{
    if (valid())
    {
        vkDestroySurfaceKHR(instance, mHandle, nullptr);
    }
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

VkSurfaceFormatKHR Surface::getSurfaceFormat(VkPhysicalDevice physicalDevice)
{
    VkSurfaceFormatKHR surfaceFormat;
    // Get list of supported surface formats
    uint32_t formatCount;
    vk_call(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, mHandle, &formatCount, NULL));
    assert(formatCount > 0);

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vk_call(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, mHandle, &formatCount, surfaceFormats.data()));

    // If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
    // there is no preferred format, so we assume VK_FORMAT_B8G8R8A8_UNORM
    if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
    {
        surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
        surfaceFormat.colorSpace = surfaceFormats[0].colorSpace;
    }
    else
    {
        // iterate over the list of available surface format and
        // check for the presence of VK_FORMAT_B8G8R8A8_UNORM
        bool found_rgba8 = false;
        for (auto&& surfaceformat : surfaceFormats)
        {
            if (surfaceformat.format == VK_FORMAT_B8G8R8A8_UNORM)
            {
                surfaceFormat = surfaceformat;
                found_rgba8 = true;
                break;
            }
        }

        // in case VK_FORMAT_B8G8R8A8_UNORM is not available
        // select the first available color format
        if (!found_rgba8)
        {
            surfaceFormat = surfaceFormats[0];
        }
    }

    return std::move(surfaceFormat);
}
} // namespace vk