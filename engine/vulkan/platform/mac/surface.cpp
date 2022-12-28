#include "vulkan/platform/mac/surface.h"
#include "platform/mac/window.h"
#include "vulkan/core.h"

namespace vk
{
Surface* Surface::createPlatformSurface()
{
    return new mac::Surface();
}

namespace mac
{
Result Surface::init(platform::Window* window, VkInstance instance)
{
    platform::mac::Window* macWindow = reinterpret_cast<platform::mac::Window*>(window);
    vk_try(glfwCreateWindowSurface(instance, macWindow->getWindow(), nullptr, &mHandle));

    return Result::Continue;
}

std::vector<std::string> Surface::getSurfaceExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<std::string> surfaceExtensions;
    for (uint32_t i = 0; i < glfwExtensionCount; i++)
    {
        surfaceExtensions.push_back(glfwExtensions[i]);
    }
    return surfaceExtensions;
}
} // namespace mac
} // namespace vk