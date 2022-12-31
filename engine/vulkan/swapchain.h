#pragma once

#include "vulkan/core.h"

namespace vk
{
class Context;

class Swapchain : public WrappedObject<Swapchain, VkSwapchainKHR>
{
  public:
    Result init(Context* context);

    void terminate(VkDevice device);

  private:
    VkResult create(VkDevice device, const VkSwapchainCreateInfoKHR& createInfo);
};
} // namespace vk