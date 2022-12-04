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

class Context : public rhi::Context
{
  public:
    Context();

    void destroy();

    Result initRHI() override final;

    void terminateRHI() override final;

  private:
    Result initInstance();

    Result initPhysicalDevice();

    Result initLogicalDevice();

  private:
    handle::Instance instance;
    handle::Device device;
    handle::PhysicalDevice physicalDevice;
    Surface* surface;

    std::vector<InstanceExtension*> instanceExtensions;
    std::vector<DeviceExtension*> deviceExtensions;

    VkPhysicalDeviceProperties2 physicalDeviceProperties2;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2;
};
} // namespace vk