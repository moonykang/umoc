#include "vulkan/swapchain.h"
#include "vulkan/device.h"
#include "vulkan/physical_device.h"
#include "vulkan/surface.h"
#include <vector>

namespace vk
{
Result Swapchain::init(Surface* surface, PhysicalDevice* physicalDevice, Device* device)
{
    VkSwapchainKHR oldSwapchain = std::move(mHandle);

    VkSurfaceCapabilitiesKHR surfaceCapabilities = surface->getSurfaceCapabilities(physicalDevice->getHandle());
    std::vector<VkPresentModeKHR> presentModes = surface->getSurfacePresentModes(physicalDevice->getHandle());

    VkExtent2D swapchainExtent = surfaceCapabilities.currentExtent;

    uint32_t desiredNumberOfSwapchainImages = surfaceCapabilities.minImageCount + 1;
    if ((surfaceCapabilities.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfaceCapabilities.maxImageCount))
    {
        desiredNumberOfSwapchainImages = surfaceCapabilities.maxImageCount;
    }

    VkSurfaceTransformFlagsKHR preTransform = surfaceCapabilities.currentTransform;

    // Find a supported composite alpha format (not all devices support alpha opaque)
    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    // Simply select the first composite alpha format available
    std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (auto& compositeAlphaFlag : compositeAlphaFlags)
    {
        if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag)
        {
            compositeAlpha = compositeAlphaFlag;
            break;
        };
    }

    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    VkSurfaceFormatKHR surfaceFormat = surface->getSurfaceFormat(physicalDevice->getHandle());

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = surface->getHandle();
    swapchainCreateInfo.minImageCount = desiredNumberOfSwapchainImages;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = swapchainExtent;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.presentMode = swapchainPresentMode;
    // Setting oldSwapChain to the saved handle of the previous swapchain aids in resource reuse and makes sure that we
    // can still present already acquired images
    swapchainCreateInfo.oldSwapchain = oldSwapchain;
    // Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the surface area
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.compositeAlpha = compositeAlpha;

    // Enable transfer source on swap chain images if supported
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
    {
        swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    // Enable transfer destination on swap chain images if supported
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
    {
        swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    vk_try(create(device->getHandle(), swapchainCreateInfo));

    // If an existing swap chain is re-created, destroy the old swap chain
    // This also cleans up all the presentable images
    if (oldSwapchain != VK_NULL_HANDLE)
    {
        /*
        // TODO:
        for (auto& swapchainImage : swapchainImages)
        {
            swapchainImage->destroy(device);
        }
        swapchainImages.clear();
        */
        vkDestroySwapchainKHR(device->getHandle(), oldSwapchain, nullptr);
    }

    return Result::Continue;
}

void Swapchain::terminate(VkDevice device)
{
    if (valid())
    {
        vkDestroySwapchainKHR(device, mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

VkResult Swapchain::create(VkDevice device, const VkSwapchainCreateInfoKHR& createInfo)
{
    ASSERT(!valid());
    return vkCreateSwapchainKHR(device, &createInfo, nullptr, &mHandle);
}
} // namespace vk