#include "vulkan/queue.h"
#include "vulkan/commandBuffer.h"
#include "vulkan/commandPool.h"
#include "vulkan/context.h"
#include "vulkan/device.h"
#include "vulkan/physicalDevice.h"
#include "vulkan/surface.h"

namespace vk
{
CommandBuffer* Context::getActiveCommandBuffer() const
{
    ASSERT(queueMap);
    return queueMap->getActiveCommandBuffer(this, QueueType::GraphicPresent);
}

CommandBuffer* Context::getUploadCommandBuffer() const
{
    ASSERT(queueMap);
    return queueMap->getUploadCommandBuffer(this, QueueType::GraphicPresent);
}

Result Context::submitUploadCommandBuffer()
{
    ASSERT(queueMap);
    return queueMap->getQueue(QueueType::GraphicPresent)->submitUpload(this);
}

Result Context::submitActiveCommandBuffer()
{
    ASSERT(queueMap);
    std::vector<VkSemaphore> waitSemaphores;
    std::vector<VkSemaphore> signalSemaphores;
    GarbageList garbageList;

    return queueMap->getQueue(QueueType::GraphicPresent)
        ->submitActive(this, waitSemaphores, signalSemaphores, std::move(garbageList));
}

Queue::Queue(uint32_t queueFamilyIndex) : commandPool(nullptr), queueFamilyIndex(queueFamilyIndex)
{
}

Result Queue::init(Context* context)
{
    Device* device = context->getDevice();

    getDeviceQueue(device->getHandle(), queueFamilyIndex, 0); // 0 queueIndex

    ASSERT(valid());

    commandPool = new CommandPool();
    try(commandPool->init(device->getHandle(), queueFamilyIndex));

    return Result::Continue;
}

void Queue::terminate(Context* context)
{
    waitIdle();
    TERMINATE(commandPool, context);

    release();
}

Result Queue::submitUpload(Context* context)
{
    ASSERT(valid());
    ASSERT(commandPool && commandPool->valid());

    return commandPool->submitUploadCommandBuffer(context, this);
}

Result Queue::submitActive(Context* context, std::vector<VkSemaphore>& waitSemaphores,
                           std::vector<VkSemaphore>& signalSemaphores, GarbageList&& garbageList)
{
    ASSERT(valid());
    ASSERT(commandPool && commandPool->valid());

    return commandPool->submitActiveCommandBuffer(context, this, waitSemaphores, signalSemaphores,
                                                  std::move(garbageList));
}

Result Queue::submit(CommandBuffer* commandBuffer)
{
    const uint32_t commandBufferCount = 1;
    VkCommandBuffer commandBuffers[commandBufferCount] = {commandBuffer->getHandle()};

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffers;

    vk_try(submit(submitInfo, commandBuffer->getFence()));

    return Result::Continue;
}

Result Queue::submit(CommandBuffer* commandBuffer, std::vector<VkSemaphore>& waitSemaphores,
                     std::vector<VkSemaphore>& signalSemaphores)
{
    const uint32_t commandBufferCount = 1;
    VkCommandBuffer commandBuffers[commandBufferCount] = {commandBuffer->getHandle()};

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffers;

    if (!waitSemaphores.empty())
    {
        VkPipelineStageFlags waitDstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
        submitInfo.pWaitSemaphores = waitSemaphores.data();
        submitInfo.pWaitDstStageMask = &waitDstStage;
    }

    if (!signalSemaphores.empty())
    {
        submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
        submitInfo.pSignalSemaphores = signalSemaphores.data();
    }

    vk_try(submit(submitInfo, commandBuffer->getFence()));

    return Result::Continue;
}

void Queue::getDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex)
{
    ASSERT(!valid());
    vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &mHandle);
}

VkResult Queue::submit(const VkSubmitInfo& submitInfo, VkFence fence)
{
    ASSERT(valid());
    return vkQueueSubmit(mHandle, 1, &submitInfo, fence);
}

VkResult Queue::waitIdle()
{
    ASSERT(valid());
    return vkQueueWaitIdle(mHandle);
}

VkResult Queue::present(const VkPresentInfoKHR& presentInfo)
{
    ASSERT(valid());
    return vkQueuePresentKHR(mHandle, &presentInfo);
}

QueueMap::QueueMap() : graphicsQueue(nullptr), computeQueue(nullptr)
{
}

Result QueueMap::createQueueCreateInfos(PhysicalDevice* physicalDevice, Surface* surface)
{
    ASSERT(surface->valid());
    ASSERT(physicalDevice->valid());

    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice->getHandle(), &queueFamilyCount, nullptr);
    ASSERT(queueFamilyCount > 0);

    queueFamilyProperties.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice->getHandle(), &queueFamilyCount,
                                             queueFamilyProperties.data());

    std::vector<VkBool32> supportsPresent(queueFamilyCount);
    LOGD("queueFamilyCount %u", queueFamilyCount);
    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice->getHandle(), i, surface->getHandle(), &supportsPresent[i]);
    }

    uint32_t allPurposeQueueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
    uint32_t computeTransferDedicatedQueueFlags = VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;

    int allPurposeQueueFamilyIndex = -1;
    int computeTransferDedicatedQueueFamilyIndex = -1;

    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        uint32_t queueFlags = queueFamilyProperties[i].queueFlags;
        if ((queueFlags & allPurposeQueueFlags) != 0)
        {
            allPurposeQueueFamilyIndex = i;
            allPurposeQueueFlags = 0;
            continue;
        }

        if ((queueFlags & computeTransferDedicatedQueueFlags) != 0)
        {
            computeTransferDedicatedQueueFamilyIndex = i;
            computeTransferDedicatedQueueFlags = 0;
            continue;
        }
    }

    ASSERT(allPurposeQueueFamilyIndex != -1);
    if (allPurposeQueueFamilyIndex == -1)
    {
        return Result::Fail;
    }

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = allPurposeQueueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    queueCreateInfos.push_back(queueCreateInfo);
    graphicsQueue = new Queue(allPurposeQueueFamilyIndex);
    computeQueue = graphicsQueue;

    queueFamilyIndices.push_back(allPurposeQueueFamilyIndex);

    if (computeTransferDedicatedQueueFlags != -1)
    {
        queueCreateInfo.queueFamilyIndex = computeTransferDedicatedQueueFamilyIndex;
        queueCreateInfos.push_back(queueCreateInfo);

        computeQueue = new Queue(computeTransferDedicatedQueueFamilyIndex);
        queueFamilyIndices.push_back(computeTransferDedicatedQueueFamilyIndex);
    }

    return Result::Continue;
}

Result QueueMap::initQueues(Context* context)
{
    ASSERT(graphicsQueue && computeQueue);

    try(graphicsQueue->init(context));
    try(computeQueue->init(context));

    return Result::Continue;
}

void QueueMap::terminate(Context* context)
{
    if (graphicsQueue != computeQueue)
    {
        TERMINATE(computeQueue, context);
    }
    else
    {
        computeQueue = nullptr;
    }
    TERMINATE(graphicsQueue, context);
}

const std::vector<VkDeviceQueueCreateInfo>& QueueMap::getQueueCreateInfo() const
{
    return queueCreateInfos;
}

Queue* QueueMap::getQueue(QueueType type)
{
    switch (type)
    {
    case QueueType::GraphicPresent:
        ASSERT(graphicsQueue);
        return graphicsQueue;
    case QueueType::ComputeTransfer:
        ASSERT(computeQueue);
        return computeQueue;
    default:
        UNREACHABLE();
        return nullptr;
    }
}

CommandBuffer* QueueMap::getActiveCommandBuffer(const Context* context, QueueType type)
{
    return getQueue(type)->getCommandPool()->getActiveCommandBuffer(context);
}

CommandBuffer* QueueMap::getUploadCommandBuffer(const Context* context, QueueType type)
{
    return getQueue(type)->getCommandPool()->getUploadCommandBuffer(context);
}

void QueueMap::waitAll()
{
    graphicsQueue->waitIdle();
    computeQueue->waitIdle();
}
} // namespace vk