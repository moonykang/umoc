#include "vulkan/platform/mac/window.h"
#include "common/external.h"
#include "vulkan/core.h"

namespace vk
{
platform::Window* Window::createWindow()
{
    return new mac::Window();
}

namespace mac
{
void Window::initRHI()
{
}

void Window::initSurface(VkInstance instance, Surface* surface)
{
    VKCALL(glfwCreateWindowSurface(instance, window, nullptr, &surface->getSurface()));
}
} // namespace mac
} // namespace vk