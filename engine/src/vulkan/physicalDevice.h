#pragma once

#include "vulkan/core.h"

namespace vk
{
class Context;

class PhysicalDevice final : public WrappedObject<PhysicalDevice, VkPhysicalDevice>
{
  public:
    PhysicalDevice();

    Result init(Context* context);

    void terminate();

    void getProperties2(VkPhysicalDeviceProperties2* properties);

    VkPhysicalDeviceFeatures2* getPhysicalDeviceFeatures2()
    {
        return &physicalDeviceFeatures2;
    }

    void** getFeatureChain(const void*& pNext)
    {
        pNext = &physicalDeviceFeatures2;
        physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

        return &physicalDeviceFeatures2.pNext;
    }

    void** getPropertyChain()
    {
        physicalDeviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;

        return &physicalDeviceProperties2.pNext;
    }

    uint32_t getPhysicalDeviceMemoryTypeIndex(const uint32_t memoryTypeBits,
                                              const VkMemoryPropertyFlags memoryProperty);

  private:
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceProperties2 physicalDeviceProperties2;
    VkPhysicalDeviceLimits physicalDeviceLimit;
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2;
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;

    // Limit
  public:
    VkPhysicalDeviceLimits getPhysicalDeviceLimits();
};
} // namespace vk
