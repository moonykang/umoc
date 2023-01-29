#include "vulkan/platform/glfw/surface.h"
#include "platform/glfw/window.h"
#include "vulkan/context.h"
#include "vulkan/core.h"
#include "vulkan/instance.h"

namespace vk
{
Surface* Surface::createPlatformSurface()
{
    return new glfw::Surface();
}

namespace glfw
{
Result Surface::init(platform::Window* window, Context* context)
{
    Instance* instance = context->getInstance();
    platform::glfw::Window* glfwWindow = reinterpret_cast<platform::glfw::Window*>(window);
    vk_try(glfwCreateWindowSurface(instance->getHandle(), glfwWindow->getWindow(), nullptr, &mHandle));

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