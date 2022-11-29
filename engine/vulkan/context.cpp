#include "context.h"
#include "common/util.h"
#include "vulkan/platform/window.h"

namespace vk
{
void Context::initRHI()
{
    LOGD("init vulkan RHI");
    window = Window::createWindow();
    window->init();
}

void Context::initInstance()
{
    /*
        VkInstanceCreateInfo instanceCreateInfo = {};

        VkApplicationInfo applicationInfo = {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "dev.samsung.littlerenderer";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
        applicationInfo.pEngineName = "dev.samsung.littlerenderer";
        applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    #if VK_USE_PLATFORM_WIN32_KHR
        applicationInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 2, 0);
    #else
        applicationInfo.apiVersion = VK_API_VERSION_1_2;
    #endif

        std::vector<const char*> requestedExtensions;

        instanceExtensions.push_back(
            ExtensionFactory::createInstanceExtension(ExtensionName::PhysicalDeviceProperties2Extension));

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> supportedExtensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportedExtensions.data());

        for (auto& extension : instanceExtensions)
        {
            extension->check(supportedExtensions);
            extension->add(requestedExtensions);
        }

        for (auto& extension : supportedExtensions)
        {
            bool notExist = true;

            // TODO
            for (auto requestedExtension : requestedExtensions)
            {
                if (std::string(requestedExtension) == std::string(extension.extensionName))
                {
                    notExist = false;
                }
            }

            if (notExist)
            {
                requestedExtensions.push_back(extension.extensionName);
                LOGD("%s", extension.extensionName);
            }
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

        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requestedExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = requestedExtensions.data();

        VKCALL(instance.init(instanceCreateInfo));

        for (auto& extension : instanceExtensions)
        {
            extension->fetch(instance.getHandle());
        }

        LOGD("Done to create instance");
    */
}
} // namespace vk