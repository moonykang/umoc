#pragma once

#include "sync.h"
#include "vulkan/core.h"
#include "vulkan/resources.h"
#include <vector>

namespace vk
{
class Context;
class Image;
class Queue;

class SwapchainSemaphore
{
  public:
    Result init(VkDevice device);

    void terminate(VkDevice device);

    Semaphore acquireSemaphore;
    Semaphore presentSemaphore;
};

class Swapchain : public WrappedObject<Swapchain, VkSwapchainKHR>
{
  public:
    Swapchain();

    Result init(Context* context);

    void terminate(VkDevice device);

    Result acquireNextImage(Context* context);

    Result present(Context* context, Queue* queue);

    Image* getCurrentSurfaceImage();

  private:
    VkResult create(VkDevice device, const VkSwapchainCreateInfoKHR& createInfo);

    Result setupSwapchainImages(Context* context, Format format, VkExtent2D extent);

    void releaseSwapchainImages(VkDevice device);

    Result setupSwapchainSemaphores(Context* context);

    void releaseSwapchainSemaphores(VkDevice device);

  private:
    std::vector<Image*> swapchainImages;
    std::vector<SwapchainSemaphore> semaphores;
    uint32_t imageCount;
    uint32_t currentImageIndex;
};
} // namespace vk