#pragma once

#include "rhi/context.h"
#include "vulkan/core.h"
#include "vulkan/surface.h"
#include <vector>

namespace vk
{
class DeviceExtension;
class InstanceExtension;
class Surface;

namespace debug
{
class DebugCallback;
}

class Context : public rhi::Context
{
  public:
    Context();

    void destroy();

    Result initRHI(platform::Window* window) override final;

    void terminateRHI() override final;

  private:
    Result initInstance();

    Result initPhysicalDevice();

    Result initLogicalDevice();

  private:
    VkDeviceQueueCreateInfo getQueueCreateInfo(VkQueueFlags queueFlags, uint32_t* queueIndex, float queuePriority = 0);

    uint32_t getQueueFamilyIndex(VkQueueFlagBits queueFlags) const;

  private:
    handle::Instance instance;
    handle::Device device;
    handle::PhysicalDevice physicalDevice;
    Surface* surface;
    debug::DebugCallback* debugCallback;

    std::vector<InstanceExtension*> instanceExtensions;
    std::vector<DeviceExtension*> deviceExtensions;

    VkPhysicalDeviceProperties2 physicalDeviceProperties2;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2;

  private:
    bool enableValidationLayer;
};
} // namespace vk