#include "vulkan/physical_device.h"
#include "vulkan/context.h"
#include "vulkan/instance.h"

namespace vk
{
PhysicalDevice::PhysicalDevice() : physicalDeviceFeatures2(), physicalDeviceProperties2(), physicalDeviceProperties()
{
}

Result PhysicalDevice::init(Context* context)
{
    LOGD("Init physical device");

    Instance* instance = context->getInstance();

    // Physical device
    uint32_t gpuCount = 0;
    // Get number of available physical devices
    vk_try(vkEnumeratePhysicalDevices(instance->getHandle(), &gpuCount, nullptr));

    LOGD("Number of GPUs : %u", gpuCount);
    if (gpuCount == 0)
    {
        LOGE("No device with Vulkan support found");
        return Result::Fail;
    }

    // Enumerate devices
    std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
    vk_try(vkEnumeratePhysicalDevices(instance->getHandle(), &gpuCount, physicalDevices.data()));

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

uint32_t PhysicalDevice::getPhysicalDeviceMemoryTypeIndex(const uint32_t memoryTypeBits,
                                                          const VkMemoryPropertyFlags memoryProperty)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(mHandle, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((memoryTypeBits & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & memoryProperty) == memoryProperty)
        {
            return i;
        }
    }

    UNREACHABLE();
    return 0;
}
} // namespace vk