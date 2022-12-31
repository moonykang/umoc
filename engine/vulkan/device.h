#pragma once

#include "vulkan/core.h"

namespace vk
{
class Context;
class DeviceExtension;
class QueueMap;

class Device final : public WrappedObject<Device, VkDevice>
{
  public:
    Device() = default;

    Result init(Context* context, QueueMap* queueMap);

    void terminate();

  private:
    VkResult create(VkPhysicalDevice device, const VkDeviceCreateInfo& createInfo);

    std::vector<VkExtensionProperties> enumerateDeviceExtensions(VkPhysicalDevice physicalDevice);

  private:
    std::map<ExtensionName, DeviceExtension*> deviceExtensions;
};
} // namespace vk