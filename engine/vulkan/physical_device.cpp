#include "vulkan/physical_device.h"

namespace vk
{
PhysicalDevice::PhysicalDevice() : physicalDeviceFeatures2(), physicalDeviceProperties2(), physicalDeviceProperties()
{
}

Result PhysicalDevice::init(VkInstance instance)
{
    LOGD("Init physical device");
    // Physical device
    uint32_t gpuCount = 0;
    // Get number of available physical devices
    vk_try(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr));

    LOGD("Number of GPUs : %u", gpuCount);
    if (gpuCount == 0)
    {
        LOGE("No device with Vulkan support found");
        return Result::Fail;
    }

    // Enumerate devices
    std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
    vk_try(vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data()));

    uint32_t selectedDevice = 0;

#if !PLATFORM_ANDROID
    for (uint32_t i = 0; i < gpuCount; i++)
    {
        vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);
        LOGD("Device [%d]: %s", i, physicalDeviceProperties.deviceName);
        // LOGD("Type: %s", debug::physicalDeviceTypeString(physicalDeviceProperties.deviceType).c_str());
    }
#endif

    setHandle(physicalDevices[selectedDevice]);
    vkGetPhysicalDeviceProperties(getHandle(), &physicalDeviceProperties);
    vkGetPhysicalDeviceFeatures(getHandle(), &physicalDeviceFeatures2.features);

    return Result::Continue;
}

void PhysicalDevice::terminate()
{
    release();
}

void PhysicalDevice::getProperties2(VkPhysicalDeviceProperties2* properties)
{
    ASSERT(valid());
    vkGetPhysicalDeviceProperties2KHR(mHandle, properties);
}
} // namespace vk