#pragma once

#include "rhi/context.h"
#include "vulkan/core.h"
#include <map>

namespace rhi
{
class Image;
class Renderpass;
} // namespace rhi

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
class QueueMap;
class RenderTargetManager;

class Context : public rhi::Context
{
  public:
    Context();

    void terminate();

    Result initRHI(platform::Window* window) override final;

    void terminateRHI() override final;

    rhi::Image* getCurrentSurfaceImage() override;

    Result flush() override;

    Result present() override;

    Result beginRenderpass(rhi::RenderPassInfo& renderpassInfo) override final;

    Result endRenderpass() override final;

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
    QueueMap* queueMap;

    DebugUtilsMessenger* debugCallback;
    RenderTargetManager* renderTargetManager;

    std::map<ExtensionName, InstanceExtension*> instanceExtensions;
    std::map<ExtensionName, DeviceExtension*> deviceExtensions;

    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2;

  private:
    bool enableValidationLayer;
};
} // namespace vk