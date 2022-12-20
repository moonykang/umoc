#include "vulkan/device.h"
#include "vulkan/extension.h"
#include "vulkan/physical_device.h"
#include "vulkan/queue.h"

namespace vk
{
void Device::destroy()
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

Result Device::init(PhysicalDevice* physicalDevice, QueueMap* queueMap)
{
    uint32_t graphicsQueueIndex = 0;

    const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos = queueMap->getQueueCreateInfo();

    // Get list of supported extensions
    uint32_t extensionCount = 0;
    std::vector<VkExtensionProperties> supportedExtensions;
    std::vector<const char*> requestedExtensions;
    vkEnumerateDeviceExtensionProperties(physicalDevice->getHandle(), nullptr, &extensionCount, nullptr);
    supportedExtensions.resize(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice->getHandle(), nullptr, &extensionCount,
                                         supportedExtensions.data());

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
    // createDeviceExtension(ExtensionName::DebugMarker);
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

    vk_try(create(physicalDevice->getHandle(), deviceCreateInfo));

    for (auto& deviceExtension : deviceExtensions)
    {
        deviceExtension.second->fetch(getHandle());
    }

    // physicalDevice.getProperties2(&physicalDeviceProperties2);

    // queueFamilyIndex = graphicsQueueIndex;
    // commandBufferManager->init(device.getHandle(), graphicsQueueIndex);
    // queue->init(device.getHandle(), graphicsQueueIndex);

    LOGD("Done to create logical device");
    /*
        VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
        pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        VKCALL(pipelineCache.init(device.getHandle(), pipelineCacheCreateInfo));
    */
    return Result::Continue;
}
} // namespace vk