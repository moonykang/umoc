#include "vulkan/context.h"
#include "vulkan/debug.h"
#include "vulkan/extension.h"
#include "vulkan/surface.h"

namespace vk
{
Context::Context() : surface(nullptr), enableValidationLayer(true), debugCallback(nullptr)
{
}

Result Context::initRHI(platform::Window* window)
{
    LOGD("init vulkan RHI");

    surface = Surface::createPlatformSurface();

    try(initInstance());

    try(surface->initSurface(window, instance.getHandle()));

    if (enableValidationLayer)
    {
        debugCallback = new debug::DebugCallback();
        debugCallback->init(instance.getHandle(), VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT,
                            VK_NULL_HANDLE);
    }

    try(initPhysicalDevice());

    try(initLogicalDevice());

    return Result::Continue;
}

void Context::terminateRHI()
{
    // Device dependencies

    // Instance dependencies
    DELETE(surface, instance.getHandle());
    DELETE(debugCallback, instance.getHandle());

    device.destroy();
    physicalDevice.release();
    instance.destroy();
    LOGD("End of terminate RHI");
}

Result Context::initInstance()
{
    VkInstanceCreateInfo instanceCreateInfo = {};

    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "Yalo";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    applicationInfo.pEngineName = "Yalo";
    applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
#if VK_USE_PLATFORM_WIN32_KHR
    applicationInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 2, 0);
#else
    applicationInfo.apiVersion = VK_API_VERSION_1_0;
#endif

    // Instance extensions
    auto createInstanceExtension = [this](ExtensionName extensionName) {
        this->instanceExtensions.try_emplace(extensionName, ExtensionFactory::createInstanceExtension(extensionName));
    };

    createInstanceExtension(ExtensionName::PhysicalDeviceProperties2);
    createInstanceExtension(ExtensionName::PortabilityEnumeration);
    createInstanceExtension(ExtensionName::DebugUtils);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> supportedExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportedExtensions.data());

    std::vector<const char*> requestedExtensions;
    for (auto& extension : instanceExtensions)
    {
        extension.second->check(supportedExtensions);
        extension.second->add(requestedExtensions);
    }

    std::vector<std::string> surfaceExtensions = surface->getSurfaceExtensions();
    for (auto& extension : surfaceExtensions)
    {
        requestedExtensions.push_back(extension.c_str());
    }

    if (enableValidationLayer)
    {
        LOGD("Finding layers...");
        const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
        {
            uint32_t instanceLayerCount;
            vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
            std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerCount);
            vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties.data());
            bool validationLayerPresent = false;
            for (VkLayerProperties layer : instanceLayerProperties)
            {
                LOGD("Layers : %s", layer.layerName);

                if (strcmp(layer.layerName, validationLayerName) == 0)
                {
                    validationLayerPresent = true;
                    break;
                }
            }
            if (validationLayerPresent)
            {
                instanceCreateInfo.ppEnabledLayerNames = &validationLayerName;
                instanceCreateInfo.enabledLayerCount = 1;
            }
            else
            {
                LOGE("Validation layer VK_LAYER_KHRONOS_validation not present, "
                     "validation is disabled");
            }
        }
    }

    LOGD("==== List of enabled instance extensions ====");
    for (auto& extension : requestedExtensions)
    {
        LOGD("%s", extension);
    }
    LOGD("==== End of enabled instance extensions ====");

    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#if PLATFORM_MAC
    instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    LOGD("instanceCreateInfo.flags: %u", instanceCreateInfo.flags);
#endif
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requestedExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = requestedExtensions.data();

    vk_try(instance.init(instanceCreateInfo));

    for (auto& extension : instanceExtensions)
    {
        extension.second->fetch(instance.getHandle());
    }

    LOGD("Done to create instance %p", instance.getHandle());

    return Result::Continue;
}

Result Context::initPhysicalDevice()
{
    LOGD("Init physical device");
    // Physical device
    uint32_t gpuCount = 0;
    // Get number of available physical devices
    vk_try(vkEnumeratePhysicalDevices(instance.getHandle(), &gpuCount, nullptr));

    LOGD("Number of GPUs : %u", gpuCount);
    if (gpuCount == 0)
    {
        LOGE("No device with Vulkan support found");
        return Result::Fail;
    }

    // Enumerate devices
    std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
    vk_try(vkEnumeratePhysicalDevices(instance.getHandle(), &gpuCount, physicalDevices.data()));

    uint32_t selectedDevice = 0;

#if !PLATFORM_ANDROID
    for (uint32_t i = 0; i < gpuCount; i++)
    {
        vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);
        LOGD("Device [%d]: %s", i, physicalDeviceProperties.deviceName);
        // LOGD("Type: %s", debug::physicalDeviceTypeString(physicalDeviceProperties.deviceType).c_str());
    }
#endif

    physicalDevice.setHandle(physicalDevices[selectedDevice]);
    vkGetPhysicalDeviceProperties(physicalDevice.getHandle(), &physicalDeviceProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice.getHandle(), &physicalDeviceFeatures2.features);

    // gpuName = std::string(physicalDeviceProperties.deviceName);

    LOGD("Physical Device %s", physicalDeviceProperties.deviceName);
    LOGD("Tessellation shader %d", physicalDeviceFeatures2.features.tessellationShader);
    LOGD("Sampler Anisotropy %d", (int)physicalDeviceFeatures2.features.samplerAnisotropy);

    return Result::Continue;
}

uint32_t Context::getQueueFamilyIndex(VkQueueFlagBits queueFlags) const
{
    ASSERT(surface->valid());
    ASSERT(physicalDevice.valid());

    std::vector<VkQueueFamilyProperties> queueFamilyProperties;

    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice.getHandle(), &queueFamilyCount, nullptr);
    assert(queueFamilyCount > 0);
    queueFamilyProperties.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice.getHandle(), &queueFamilyCount,
                                             queueFamilyProperties.data());

    std::vector<VkBool32> supportsPresent(queueFamilyCount);
    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice.getHandle(), i, surface->getSurface(), &supportsPresent[i]);
    }

    // Dedicated queue for compute
    // Try to find a queue family index that supports compute but not graphics
    if (queueFlags & VK_QUEUE_COMPUTE_BIT)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
        {
            if ((queueFamilyProperties[i].queueFlags & queueFlags) &&
                ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
            {
                return i;
            }
        }
    }

    // Dedicated queue for transfer
    // Try to find a queue family index that supports transfer but not graphics
    // and compute
    if (queueFlags & VK_QUEUE_TRANSFER_BIT)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
        {
            if ((queueFamilyProperties[i].queueFlags & queueFlags) &&
                ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
                ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
            {
                return i;
            }
        }
    }

    // For other queue types or if no separate compute queue is present, return
    // the first one to support the requested flags
    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
    {
        if (queueFamilyProperties[i].queueFlags & queueFlags)
        {
            return i;
        }
    }

    throw std::runtime_error("Could not find a matching queue family index");
}

VkDeviceQueueCreateInfo Context::getQueueCreateInfo(VkQueueFlags queueFlags, uint32_t* queueIndex, float queuePriority)
{
    *queueIndex = getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = *queueIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    return queueCreateInfo;
}

Result Context::initLogicalDevice()
{
    uint32_t graphicsQueueIndex = 0;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    queueCreateInfos.push_back(getQueueCreateInfo(VK_QUEUE_GRAPHICS_BIT, &graphicsQueueIndex));

    // Get list of supported extensions
    uint32_t extensionCount = 0;
    std::vector<VkExtensionProperties> supportedExtensions;
    std::vector<const char*> requestedExtensions;
    vkEnumerateDeviceExtensionProperties(physicalDevice.getHandle(), nullptr, &extensionCount, nullptr);
    supportedExtensions.resize(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice.getHandle(), nullptr, &extensionCount,
                                         supportedExtensions.data());

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    deviceCreateInfo.pNext = &physicalDeviceFeatures2;
    physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    void** nextFeatureChain = &physicalDeviceFeatures2.pNext;

    VkPhysicalDeviceProperties2 physicalDeviceProperties2 = {};
    physicalDeviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;
    void** nextPropertyChain = &physicalDeviceProperties2.pNext;

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

    vk_try(device.init(physicalDevice.getHandle(), deviceCreateInfo));

    for (auto& deviceExtension : deviceExtensions)
    {
        deviceExtension.second->fetch(device.getHandle());
    }

    physicalDevice.getProperties2(&physicalDeviceProperties2);

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