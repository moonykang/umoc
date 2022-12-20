#pragma once

#include "rhi/context.h"
#include "vulkan/core.h"
#include "vulkan/device.h"
#include "vulkan/instance.h"
#include "vulkan/physical_device.h"
#include "vulkan/queue.h"
#include "vulkan/surface.h"
#include <map>

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
    Instance instance;
    Surface* surface;
    PhysicalDevice physicalDevice;
    Device device;
    QueueMap queueMap;

    debug::DebugCallback* debugCallback;

    std::map<ExtensionName, InstanceExtension*> instanceExtensions;
    std::map<ExtensionName, DeviceExtension*> deviceExtensions;

    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2;

  private:
    bool enableValidationLayer;
};
} // namespace vk