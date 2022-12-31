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

    Result init(PhysicalDevice* physicalDevice, QueueMap* queueMap);

    void terminate();

  private:
    VkResult create(VkPhysicalDevice device, const VkDeviceCreateInfo& createInfo);

    std::vector<VkExtensionProperties> enumerateDeviceExtensions(VkPhysicalDevice physicalDevice);

  private:
    std::map<ExtensionName, DeviceExtension*> deviceExtensions;
};
} // namespace vk