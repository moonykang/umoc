#include "vulkan/context.h"
#include "vulkan/debug.h"
#include "vulkan/extension.h"
#include "vulkan/platform/window.h"

namespace vk
{
Context::Context() : surface(nullptr), enableValidationLayer(true), debugCallback(nullptr)
{
}

Result Context::initRHI()
{
    LOGD("init vulkan RHI");

    try(initInstance());

    if (enableValidationLayer)
    {
        debugCallback = new debug::DebugCallback();
        debugCallback->init(instance.getHandle(), VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT,
                            VK_NULL_HANDLE);
    }

    try(initPhysicalDevice());

    window = Window::createWindow();
    window->init();

    try(initLogicalDevice());

    return Result::Continue;
}

void Context::terminateRHI()
{
    if (surface)
    {
    }

    window->terminate();

    DELETE(debugCallback, instance.getHandle());

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

    instanceExtensions.push_back(
        ExtensionFactory::createInstanceExtension(ExtensionName::PhysicalDeviceProperties2Extension));
    instanceExtensions.push_back(
        ExtensionFactory::createInstanceExtension(ExtensionName::PortabilityEnumerationExtension));
    instanceExtensions.push_back(ExtensionFactory::createInstanceExtension(ExtensionName::DebugUtilsExtension));

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> supportedExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportedExtensions.data());

    std::vector<const char*> requestedExtensions;
    for (auto& extension : instanceExtensions)
    {
        extension->check(supportedExtensions);
        extension->add(requestedExtensions);
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
        extension->fetch(instance.getHandle());
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

Result Context::initLogicalDevice()
{
    return Result::Continue;
}
} // namespace vk