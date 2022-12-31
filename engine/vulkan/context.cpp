#include "vulkan/context.h"
#include "vulkan/debug.h"
#include "vulkan/extension.h"
#include "vulkan/surface.h"
#include "vulkan/swapchain.h"

namespace vk
{
Context::Context() : surface(nullptr), enableValidationLayer(true), debugCallback(nullptr)
{
}

Result Context::initRHI(platform::Window* window)
{
    LOGD("init vulkan RHI");

    surface = Surface::createPlatformSurface();
    try(instance.init(surface));

    try(surface->init(window, instance.getHandle()));

    if (enableValidationLayer)
    {
        debugCallback = new debug::DebugUtilsMessenger();
        debugCallback->init(instance.getHandle(), VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT,
                            VK_NULL_HANDLE);
    }

    try(physicalDevice.init(instance.getHandle()));

    try(queueMap.createQueueCreateInfos(&physicalDevice, surface));

    try(device.init(&physicalDevice, &queueMap));

    swapchain = new Swapchain();
    try(swapchain->init(surface, &physicalDevice, &device));

    return Result::Continue;
}

void Context::terminateRHI()
{
    // Device dependencies
    DELETE(swapchain, device.getHandle());
    device.terminate();

    // Instance dependencies
    DELETE(surface, instance.getHandle());
    DELETE(debugCallback, instance.getHandle());

    physicalDevice.release();
    instance.terminate();
    LOGD("End of terminate RHI");
}
} // namespace vk