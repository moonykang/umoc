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
class DebugUtilsMessenger;

class Context : public rhi::Context
{
  public:
    Context();

    void terminate();

    Result initRHI(platform::Window* window) override final;

    void terminateRHI() override final;

  public:
    Instance* getInstance() const;

    PhysicalDevice* getPhysicalDevice() const;

    Device* getDevice() const;

    Surface* getSurface() const;

    Swapchain* getSwapchain() const;

  private:
    Instance* instance;
    Surface* surface;
    PhysicalDevice* physicalDevice;
    Device* device;
    Swapchain* swapchain;
    QueueMap queueMap;

    DebugUtilsMessenger* debugCallback;

    std::map<ExtensionName, InstanceExtension*> instanceExtensions;
    std::map<ExtensionName, DeviceExtension*> deviceExtensions;

    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2;

  private:
    bool enableValidationLayer;
};
} // namespace vk