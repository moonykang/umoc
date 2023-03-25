#include "vulkan/commandPool.h"
#include "context.h"
#include "device.h"
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

void CommandPool::terminate(Context* context)
{
    VkDevice device = context->getDevice()->getHandle();

    while (!submitCommandBuffers.empty())
    {
        CommandBufferAndGarbage& entry = submitCommandBuffers.front();
        auto commandBuffer = entry.getCommandBuffer();

        // TODO
        for (auto& garbage : entry.getGarbage())
        {
            garbage.terminate(context);
        }
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

CommandBuffer* CommandPool::getActiveCommandBuffer(const Context* context)
{
    ASSERT(valid());
    if (activeCommandBuffer == nullptr)
    {
        prepareActiveCommandBuffer(context->getDevice()->getHandle());
        activeCommandBuffer->begin();
    }
    return activeCommandBuffer;
}

CommandBuffer* CommandPool::getUploadCommandBuffer(const Context* context)
{
    ASSERT(valid());
    if (uploadCommandBuffer == nullptr)
    {
        prepareUploadCommandBuffer(context->getDevice()->getHandle());
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

Result CommandPool::submitActiveCommandBuffer(Context* context, Queue* queue, std::vector<VkSemaphore>& waitSemaphores,
                                              std::vector<VkSemaphore>& signalSemaphores, GarbageList&& garbageList)
{
    ASSERT(activeCommandBuffer);
    if (uploadCommandBuffer)
    {
        try(submitUploadCommandBuffer(context, queue));
    }
    activeCommandBuffer->end();

    try(queue->submit(activeCommandBuffer, waitSemaphores, signalSemaphores));

    resetCommandBuffers(context);

    submitCommandBuffers.push({std::move(garbageList), activeCommandBuffer});
    activeCommandBuffer = nullptr;

    return Result::Continue;
}

Result CommandPool::submitUploadCommandBuffer(Context* context, Queue* queue)
{
    if (!uploadCommandBuffer)
    {
        return Result::Continue;
    }

    uploadCommandBuffer->end();
    try(queue->submit(uploadCommandBuffer));

    resetCommandBuffers(context);
    submitCommandBuffers.push(uploadCommandBuffer);
    uploadCommandBuffer = nullptr;

    return Result::Continue;
}

void CommandPool::resetCommandBuffers(Context* context, bool bIdle)
{
    while (!submitCommandBuffers.empty())
    {
        CommandBufferAndGarbage& entry = submitCommandBuffers.front();
        auto commandBuffer = entry.getCommandBuffer();

        const bool forceWait = submitCommandBuffers.size() > kMaxInFlightCommandBuffers || bIdle;

        if (commandBuffer->reset(context->getDevice()->getHandle(), forceWait) == Result::Continue)
        {
            for (auto& garbage : entry.getGarbage())
            {
                garbage.terminate(context);
            }

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
