#include "vulkan/context.h"
#include "buffer.h"
#include "pipeline.h"
#include "rendertarget.h"
#include "vulkan/device.h"
#include "vulkan/extension.h"
#include "vulkan/image.h"
#include "vulkan/instance.h"
#include "vulkan/physicalDevice.h"
#include "vulkan/queue.h"
#include "vulkan/surface.h"
#include "vulkan/swapchain.h"

namespace vk
{
Context::Context()
    : device(nullptr), instance(nullptr), physicalDevice(nullptr), surface(nullptr), swapchain(nullptr),
      enableValidationLayer(true), debugCallback(nullptr), shaderMap(nullptr), pipelineMap(nullptr)
{
}

Result Context::initRHIImplementation(platform::Window* window)
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

    queueMap = new QueueMap();
    try(queueMap->createQueueCreateInfos(physicalDevice, surface));

    device = new Device();
    try(device->init(this, queueMap));
    try(queueMap->initQueues(this));

    swapchain = new Swapchain();
    try(swapchain->init(this));

    renderTargetManager = new RenderTargetManager();

    shaderMap = new ShaderMap();

    pipelineMap = new PipelineMap();

    return Result::Continue;
}

void Context::terminateRHIImplementation()
{
    LOGD("Begin of terminate Vulkan RHI");
    queueMap->waitAll();

    // Device dependencies
    TERMINATE(shaderMap, device->getHandle());
    TERMINATE(pipelineMap, device->getHandle());
    TERMINATE(renderTargetManager, device->getHandle());
    TERMINATE(swapchain, device->getHandle());
    TERMINATE(queueMap, device->getHandle());
    TERMINATE(device);

    // Instance dependencies
    TERMINATE(surface, instance->getHandle());
    TERMINATE(debugCallback, instance->getHandle());

    TERMINATE(physicalDevice);
    TERMINATE(instance);
    LOGD("End of terminate Vulkan RHI");
}

Result Context::flush()
{
    // Queue* queue = queueMap->getQueue();

    return Result::Continue;
}

Result Context::present()
{
    // TODO
    try(swapchain->acquireNextImage(this));

    Queue* presentQueue = queueMap->getQueue(QueueType::GraphicPresent);

    try(swapchain->present(this, presentQueue));

    vk_try(presentQueue->waitIdle());

    return Result::Continue;
}

Result Context::waitIdle()
{
    queueMap->waitAll();

    return Result::Continue;
}

rhi::Image* Context::getCurrentSurfaceImage()
{
    return swapchain->getCurrentSurfaceImage();
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

QueueMap* Context::getQueueMap() const
{
    ASSERT(queueMap);
    return queueMap;
}
} // namespace vk