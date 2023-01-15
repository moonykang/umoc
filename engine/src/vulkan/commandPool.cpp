#include "vulkan/commandPool.h"
#include "vulkan/commandBuffer.h"
#include "vulkan/queue.h"
#include <vector>

namespace vk
{
CommandPool::CommandPool() : uploadCommandBuffer(nullptr), activeCommandBuffer(nullptr)
{
}

Result CommandPool::init(VkDevice device, uint32_t queueFamilyIndex)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    vk_try(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &mHandle));

    try(allocateCommandBuffers(device));

    return Result::Continue;
}

void CommandPool::terminate(VkDevice device)
{
    while (!submitCommandBuffers.empty())
    {
        auto& commandBuffer = submitCommandBuffers.front();
        submitCommandBuffers.pop();

        commandBuffer->terminate(device, mHandle);
        delete commandBuffer;
    }

    while (!readyCommandBuffers.empty())
    {
        auto& commandBuffer = readyCommandBuffers.front();
        readyCommandBuffers.pop();

        commandBuffer->terminate(device, mHandle);
        delete commandBuffer;
    }

    if (valid())
    {
        vkDestroyCommandPool(device, mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

Result CommandPool::allocateCommandBuffers(VkDevice device)
{
    for (uint32_t i = 0; i < DEFAULT_NUM_COMMAND_BUFFER; i++)
    {
        CommandBuffer* commandBuffer = new CommandBuffer();
        commandBuffer->init(device, mHandle);
        readyCommandBuffers.push(commandBuffer);
    }

    return Result::Continue;
}

void CommandPool::prepareActiveCommandBuffer(VkDevice device)
{
    if (activeCommandBuffer == nullptr)
    {
        if (readyCommandBuffers.empty())
        {
            allocateCommandBuffers(device);
        }

        activeCommandBuffer = readyCommandBuffers.front();
        readyCommandBuffers.pop();
    }
}

void CommandPool::prepareUploadCommandBuffer(VkDevice device)
{
    if (uploadCommandBuffer == nullptr)
    {
        if (readyCommandBuffers.empty())
        {
            allocateCommandBuffers(device);
        }

        uploadCommandBuffer = readyCommandBuffers.front();
        readyCommandBuffers.pop();
    }
}

CommandBuffer* CommandPool::getActiveCommandBuffer(VkDevice device)
{
    ASSERT(valid());
    if (activeCommandBuffer == nullptr)
    {
        prepareActiveCommandBuffer(device);
        activeCommandBuffer->begin();
    }
    return activeCommandBuffer;
}

CommandBuffer* CommandPool::getUploadCommandBuffer(VkDevice device)
{
    ASSERT(valid());
    if (uploadCommandBuffer == nullptr)
    {
        prepareUploadCommandBuffer(device);
        uploadCommandBuffer->begin();
    }
    return uploadCommandBuffer;
}

std::vector<VkCommandBuffer> CommandPool::prepareSubmit()
{
    std::vector<VkCommandBuffer> commandBuffersToSubmit;

    if (uploadCommandBuffer)
    {
        commandBuffersToSubmit.push_back(uploadCommandBuffer->getHandle());
    }

    if (activeCommandBuffer)
    {
        commandBuffersToSubmit.push_back(activeCommandBuffer->getHandle());
    }

    ASSERT(!commandBuffersToSubmit.empty());

    return commandBuffersToSubmit;
}

Result CommandPool::submitActiveCommandBuffer(VkDevice device, Queue* queue, std::vector<VkSemaphore>* waitSemaphores,
                                              std::vector<VkSemaphore>* signalSemaphores)
{
    ASSERT(activeCommandBuffer);
    if (uploadCommandBuffer)
    {
        try(submitUploadCommandBuffer(device, queue));
    }
    activeCommandBuffer->end();
    try(queue->submit(activeCommandBuffer, waitSemaphores, signalSemaphores));

    resetCommandBuffers(device);
    submitCommandBuffers.push(activeCommandBuffer);
    activeCommandBuffer = nullptr;

    return Result::Continue;
}

Result CommandPool::submitUploadCommandBuffer(VkDevice device, Queue* queue)
{
    ASSERT(uploadCommandBuffer);
    uploadCommandBuffer->end();
    try(queue->submit(uploadCommandBuffer));

    resetCommandBuffers(device);
    submitCommandBuffers.push(uploadCommandBuffer);
    uploadCommandBuffer = nullptr;

    return Result::Continue;
}

void CommandPool::resetCommandBuffers(VkDevice device, bool bIdle)
{
    while (!submitCommandBuffers.empty())
    {
        auto commandBuffer = submitCommandBuffers.front();

        const bool forceWait = submitCommandBuffers.size() > kMaxInFlightCommandBuffers || bIdle;

        if (commandBuffer->reset(device, forceWait) == Result::Continue)
        {
            submitCommandBuffers.pop();
            readyCommandBuffers.push(commandBuffer);
        }
        else
        {
            break;
        }
    }
}
} // namespace vk