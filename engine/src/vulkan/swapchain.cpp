#include "vulkan/swapchain.h"
#include "commandBuffer.h"
#include "commandPool.h"
#include "queue.h"
#include "vulkan/context.h"
#include "vulkan/device.h"
#include "vulkan/image.h"
#include "vulkan/physicalDevice.h"
#include "vulkan/surface.h"
#include <vector>

namespace vk
{

Result SwapchainSemaphore::init(VkDevice device)
{
    vk_try(acquireSemaphore.init(device));
    vk_try(presentSemaphore.init(device));

    return Result::Continue;
}

void SwapchainSemaphore::terminate(VkDevice device)
{
    acquireSemaphore.terminate(device);
    presentSemaphore.terminate(device);
}

Swapchain::Swapchain() : imageCount(0), currentImageIndex(0)
{
}

Result Swapchain::init(Context* context)
{
    Surface* surface = context->getSurface();
    PhysicalDevice* physicalDevice = context->getPhysicalDevice();
    Device* device = context->getDevice();

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
        releaseSwapchainImages(device->getHandle());
        vkDestroySwapchainKHR(device->getHandle(), oldSwapchain, nullptr);
    }

    setupSwapchainImages(context, {surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT}, swapchainExtent);
    setupSwapchainSemaphores(context);

    return Result::Continue;
}

void Swapchain::terminate(VkDevice device)
{
    releaseSwapchainImages(device);
    releaseSwapchainSemaphores(device);

    if (valid())
    {
        vkDestroySwapchainKHR(device, mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

Result Swapchain::acquireNextImage(Context* context)
{
    VkSemaphore semaphore = semaphores[currentImageIndex].acquireSemaphore.getHandle();

    vk_try(vkAcquireNextImageKHR(context->getDevice()->getHandle(), mHandle, UINT64_MAX, semaphore, VK_NULL_HANDLE,
                                 &currentImageIndex));
    return Result::Continue;
}

Result Swapchain::present(Context* context, Queue* queue)
{
    auto image = swapchainImages[currentImageIndex];

    CommandBuffer* commandBuffer = queue->getCommandPool()->getActiveCommandBuffer(context);
    commandBuffer->addTransition(image->updateImageLayoutAndBarrier(rhi::ImageLayout::Present));

    std::vector<VkSemaphore> waitSemaphores = {semaphores[currentImageIndex].acquireSemaphore.getHandle()};
    std::vector<VkSemaphore> signalSemaphores = {semaphores[currentImageIndex].presentSemaphore.getHandle()};

    queue->submitActive(context, &waitSemaphores, &signalSemaphores);

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &mHandle;
    presentInfo.pImageIndices = &currentImageIndex;
    presentInfo.waitSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    presentInfo.pWaitSemaphores = signalSemaphores.data();

    vk_try(queue->present(presentInfo));

    currentImageIndex = (currentImageIndex + 1) % imageCount;

    return Result::Continue;
}

Image* Swapchain::getCurrentSurfaceImage()
{
    Image* currentSwapchainImage = swapchainImages[currentImageIndex];
    ASSERT(currentSwapchainImage->valid());

    return currentSwapchainImage;
}

VkResult Swapchain::create(VkDevice device, const VkSwapchainCreateInfoKHR& createInfo)
{
    ASSERT(!valid());
    return vkCreateSwapchainKHR(device, &createInfo, nullptr, &mHandle);
}

Result Swapchain::setupSwapchainImages(Context* context, Format format, VkExtent2D extent)
{
    ASSERT(imageCount == 0);

    Device* device = context->getDevice();

    std::vector<VkImage> images;
    vk_try(vkGetSwapchainImagesKHR(device->getHandle(), mHandle, &imageCount, nullptr));

    // Get the swap chain images
    images.resize(imageCount);
    vk_try(vkGetSwapchainImagesKHR(device->getHandle(), mHandle, &imageCount, images.data()));

    for (auto& image : images)
    {
        Image* swapchainImage = new Image();
        swapchainImage->init(context, image, format, VK_IMAGE_TYPE_2D, 1, 1, 1, {extent.width, extent.height, 1},
                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
        swapchainImage->initView(context);
        swapchainImages.push_back(swapchainImage);
    }

    return Result::Continue;
}

void Swapchain::releaseSwapchainImages(VkDevice device)
{
    for (auto& swapchainImage : swapchainImages)
    {
        swapchainImage->release(device);
        delete swapchainImage;
        swapchainImage = nullptr;
    }
    swapchainImages.clear();
    imageCount = 0;
}

Result Swapchain::setupSwapchainSemaphores(Context* context)
{
    ASSERT(semaphores.empty());
    semaphores.resize(imageCount);

    for (auto& semaphore : semaphores)
    {
        semaphore.init(context->getDevice()->getHandle());
    }
    return Result::Continue;
}

void Swapchain::releaseSwapchainSemaphores(VkDevice device)
{
    for (auto& semaphore : semaphores)
    {
        semaphore.terminate(device);
    }
    semaphores.clear();
}
} // namespace vk