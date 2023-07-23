#include "vulkan/instance.h"
#include "vulkan/context.h"
#include "vulkan/core.h"
#include "vulkan/extension.h"
#include "vulkan/surface.h"

namespace vk
{
Result Instance::init(Context* context)
{
    Surface* surface = context->getSurface();

    VkInstanceCreateInfo instanceCreateInfo = {};

    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "Umoc";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    applicationInfo.pEngineName = "Umoc";
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
    createInstanceExtension(ExtensionName::DebugReport);

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

    const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
    const char* renderdocLayerName = "VK_LAYER_RENDERDOC_Capture";

    std::vector<const char*> requestedLayers;
    std::vector<const char*> enabledLayers;

    // requestedLayers.push_back(validationLayerName);
    requestedLayers.push_back(renderdocLayerName);

    {
        LOGD("Finding layers...");

        uint32_t instanceLayerCount;
        vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
        std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerCount);
        vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties.data());

        for (auto requestedLayer : requestedLayers)
        {
            for (VkLayerProperties layer : instanceLayerProperties)
            {
                if (strcmp(layer.layerName, requestedLayer) == 0)
                {
                    LOGD("Layers : %s", layer.layerName);
                    enabledLayers.push_back(requestedLayer);
                    break;
                }
            }
        }

        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledLayers.size());
        instanceCreateInfo.ppEnabledExtensionNames = enabledLayers.data();
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

    vk_try(create(instanceCreateInfo));

    for (auto& extension : instanceExtensions)
    {
        extension.second->fetch(getHandle());
    }

    LOGD("Done to create instance %p", getHandle());

    return Result::Continue;
}

void Instance::terminate()
{
    if (valid())
    {
        vkDestroyInstance(mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

VkResult Instance::create(const VkInstanceCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateInstance(&createInfo, nullptr, &mHandle);
}
} // namespace vk