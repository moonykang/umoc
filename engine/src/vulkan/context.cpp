#include "vulkan/context.h"
#include "buffer.h"
#include "commandBuffer.h"
#include "debug.h"
#include "descriptor.h"
#include "pendingState.h"
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
      enableValidationLayer(true), debugCallback(nullptr), pipelineMap(nullptr), emptyDescriptorSetLayout(nullptr),
      pendingState(nullptr)
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

    pipelineMap = new PipelineMap();
    try(initEmptyDescriptorSetLayout());

    descriptorPool = new DescriptorPool();
    try(descriptorPool->init(this));

    pendingState = new PendingState();

    return Result::Continue;
}

void Context::terminateRHIImplementation()
{
    LOGD("Begin of terminate Vulkan RHI");
    queueMap->waitAll();

    // Device dependencies
    TERMINATE(emptyDescriptorSetLayout, this);
    TERMINATE(descriptorPool, this);
    TERMINATE(pipelineMap, device->getHandle());
    TERMINATE(renderTargetManager, device->getHandle());
    TERMINATE(swapchain, this);
    TERMINATE(queueMap, this);

    if (pendingState)
    {
        delete pendingState;
        pendingState = nullptr;
    }

    clearAllGarbage();
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
    Queue* presentQueue = queueMap->getQueue(QueueType::GraphicPresent);
    // Queue* queue = queueMap->getQueue();
    return submitActiveCommandBuffer();
}

Result Context::present()
{
    // TODO
    try(swapchain->acquireNextImage(this));

    Queue* presentQueue = queueMap->getQueue(QueueType::GraphicPresent);

    try(swapchain->present(this, presentQueue, std::move(garbageList)));

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

rhi::Extent3D Context::getSurfaceSize()
{
    VkExtent2D extent = swapchain->getSize();
    return {extent.width, extent.height, 1};
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

void Context::clearAllGarbage()
{
    for (auto& garbage : garbageList)
    {
        garbage.terminate(this);
    }
    garbageList.clear();
}

Result Context::flushTransition()
{
    CommandBuffer* commandBuffer = getActiveCommandBuffer();
    commandBuffer->flushTransitions();

    return Result::Continue;
}
} // namespace vk
