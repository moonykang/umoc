#pragma once

#include "vulkan/core.h"

namespace vk
{
class DeviceExtension;
class PhysicalDevice;
class QueueMap;

class Device final : public WrappedObject<Device, VkDevice>
{
  public:
    Device() = default;

    void destroy();

    VkResult create(VkPhysicalDevice device, const VkDeviceCreateInfo& createInfo);

    Result init(PhysicalDevice* physicalDevice, QueueMap* queueMap);

  private:
    std::map<ExtensionName, DeviceExtension*> deviceExtensions;
};
} // namespace vk