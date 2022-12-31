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
    try(instance->init(this));

    try(surface->init(window, this));

    if (enableValidationLayer)
    {
        debugCallback = new DebugUtilsMessenger();
        debugCallback->init(this, VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT, VK_NULL_HANDLE);
    }

    physicalDevice = new PhysicalDevice();
    try(physicalDevice->init(this));

    try(queueMap.createQueueCreateInfos(physicalDevice, surface));

    device = new Device();
    try(device->init(this, &queueMap));

    swapchain = new Swapchain();
    try(swapchain->init(this));

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

Instance* Context::getInstance() const
{
    ASSERT(instance);
    return instance;
}

PhysicalDevice* Context::getPhysicalDevice() const
{
    ASSERT(physicalDevice);
    return physicalDevice;
}

Device* Context::getDevice() const
{
    ASSERT(device);
    return device;
}

Surface* Context::getSurface() const
{
    ASSERT(surface);
    return surface;
}

Swapchain* Context::getSwapchain() const
{
    ASSERT(swapchain);
    return swapchain;
}
} // namespace vk