#pragma once

#include "vulkan/core.h"
#include "vulkan/resources.h"
#include <vector>

namespace vk
{
class Context;
class Image;

class Swapchain : public WrappedObject<Swapchain, VkSwapchainKHR>
{
  public:
    Result init(Context* context);

    void terminate(VkDevice device);

  private:
    VkResult create(VkDevice device, const VkSwapchainCreateInfoKHR& createInfo);

    Result setupSwapchainImages(Context* context, Format format, VkExtent2D extent);

    void releaseSwapchainImages(VkDevice device);

  private:
    std::vector<Image*> swapchainImages;
};
} // namespace vk