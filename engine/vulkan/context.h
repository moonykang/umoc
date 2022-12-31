#pragma once

#include "rhi/context.h"
#include "vulkan/core.h"
#include "vulkan/queue.h"
#include <map>

namespace vk
{
class Device;
class DeviceExtension;
class InstanceExtension;
class Instance;
class PhysicalDevice;
class Surface;
class Swapchain;

namespace debug
{
class DebugCallback;
}

class Context : public rhi::Context
{
  public:
    Context();

    void terminate();

    Result initRHI(platform::Window* window) override final;

    void terminateRHI() override final;

  private:
    Instance* instance;
    Surface* surface;
    PhysicalDevice* physicalDevice;
    Device* device;
    Swapchain* swapchain;
    QueueMap queueMap;

    debug::DebugUtilsMessenger* debugCallback;

    std::map<ExtensionName, InstanceExtension*> instanceExtensions;
    std::map<ExtensionName, DeviceExtension*> deviceExtensions;

    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2;

  private:
    bool enableValidationLayer;
};
} // namespace vk