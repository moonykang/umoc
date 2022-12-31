#include "vulkan/context.h"
#include "vulkan/debug.h"
#include "vulkan/device.h"
#include "vulkan/extension.h"
#include "vulkan/instance.h"
#include "vulkan/physical_device.h"
#include "vulkan/surface.h"
#include "vulkan/swapchain.h"

namespace vk
{
Context::Context()
    : device(nullptr), instance(nullptr), physicalDevice(nullptr), surface(nullptr), swapchain(nullptr),
      enableValidationLayer(true), debugCallback(nullptr)
{
}

Result Context::initRHI(platform::Window* window)
{
    LOGD("init vulkan RHI");

    surface = Surface::createPlatformSurface();
    instance = new Instance();
    try(instance->init(surface));

    try(surface->init(window, instance->getHandle()));

    if (enableValidationLayer)
    {
        debugCallback = new debug::DebugUtilsMessenger();
        debugCallback->init(instance->getHandle(), VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT,
                            VK_NULL_HANDLE);
    }

    physicalDevice = new PhysicalDevice();
    try(physicalDevice->init(instance->getHandle()));

    try(queueMap.createQueueCreateInfos(physicalDevice, surface));

    device = new Device();
    try(device->init(physicalDevice, &queueMap));

    swapchain = new Swapchain();
    try(swapchain->init(surface, physicalDevice, device));

    return Result::Continue;
}

void Context::terminateRHI()
{
    // Device dependencies
    DELETE(swapchain, device->getHandle());
    DELETE(device);

    // Instance dependencies
    DELETE(surface, instance->getHandle());
    DELETE(debugCallback, instance->getHandle());

    DELETE(physicalDevice);
    DELETE(instance);
    LOGD("End of terminate RHI");
}
} // namespace vk