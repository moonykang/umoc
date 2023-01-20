#include "vulkan/instance.h"
#include "vulkan/context.h"
#include "vulkan/debug.h"
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

    if (true) // enableValidationLayer)
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