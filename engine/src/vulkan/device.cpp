#include "vulkan/device.h"
#include "vulkan/context.h"
#include "vulkan/extension.h"
#include "vulkan/physicalDevice.h"
#include "vulkan/queue.h"

namespace vk
{
Result Device::init(Context* context, QueueMap* queueMap)
{
    PhysicalDevice* physicalDevice = context->getPhysicalDevice();

    uint32_t graphicsQueueIndex = 0;

    const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos = queueMap->getQueueCreateInfo();

    // Get list of supported extensions
    uint32_t extensionCount = 0;
    std::vector<VkExtensionProperties> supportedExtensions = enumerateDeviceExtensions(physicalDevice->getHandle());
    std::vector<const char*> requestedExtensions;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    void** nextFeatureChain = physicalDevice->getFeatureChain(deviceCreateInfo.pNext);
    void** nextPropertyChain = physicalDevice->getPropertyChain();

    // Device extensions
    auto createDeviceExtension = [this](ExtensionName extensionName) {
        this->deviceExtensions.try_emplace(extensionName, ExtensionFactory::createDeviceExtension(extensionName));
    };

    createDeviceExtension(ExtensionName::Swapchain);
    createDeviceExtension(ExtensionName::AccelerationStructure);
    createDeviceExtension(ExtensionName::RayTracingPipeline);
    createDeviceExtension(ExtensionName::DeviceAddress);
    createDeviceExtension(ExtensionName::DeferredHostOperations);
    createDeviceExtension(ExtensionName::RayQuery);
    createDeviceExtension(ExtensionName::PortabilitySubset);

    if (context->supportInstanceExtension(ExtensionName::DebugReport))
    {
        createDeviceExtension(ExtensionName::DebugMarker);
    }

    // createDeviceExtension(ExtensionName::DescriptorIndexing);
    // createDeviceExtension(ExtensionName::Spirv_1_4);

    for (auto& deviceExtension : deviceExtensions)
    {
        deviceExtension.second->check(supportedExtensions);
        deviceExtension.second->add(requestedExtensions);
        deviceExtension.second->feature(nextFeatureChain);
        deviceExtension.second->property(nextPropertyChain);
    }

    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requestedExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = requestedExtensions.data();

    LOGD("==== List of enabled Device extensions ====");
    for (auto& extension : requestedExtensions)
    {
        LOGD("%s", extension);
    }
    LOGD("==== End of enabled Device extensions ====");

    ASSERT(!valid());
    vk_try(create(physicalDevice->getHandle(), deviceCreateInfo));

    for (auto& deviceExtension : deviceExtensions)
    {
        deviceExtension.second->fetch(getHandle());
    }

    LOGD("Done to create logical device");
    return Result::Continue;
}

void Device::terminate()
{
    if (valid())
    {
        vkDestroyDevice(mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

VkResult Device::create(VkPhysicalDevice device, const VkDeviceCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateDevice(device, &createInfo, nullptr, &mHandle);
}

std::vector<VkExtensionProperties> Device::enumerateDeviceExtensions(VkPhysicalDevice physicalDevice)
{
    uint32_t extensionCount = 0;
    std::vector<VkExtensionProperties> extensionProperties;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
    extensionProperties.resize(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensionProperties.data());

    return std::move(extensionProperties);
}
} // namespace vk