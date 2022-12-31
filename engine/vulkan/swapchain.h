#pragma once

#include "vulkan/core.h"

namespace vk
{
class Surface;
class Device;
class PhysicalDevice;

class Swapchain : public WrappedObject<Swapchain, VkSwapchainKHR>
{
  public:
    Result init(Surface* surface, PhysicalDevice* physicalDevice, Device* device);

    void terminate(VkDevice device);

  private:
    VkResult create(VkDevice device, const VkSwapchainCreateInfoKHR& createInfo);
};
} // namespace vk