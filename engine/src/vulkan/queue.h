#pragma once

#include "vulkan/core.h"

namespace vk
{
class Surface;
class PhysicalDevice;
class CommandPool;
class Context;
class CommandBuffer;

enum class QueueType
{
    ComputeTransfer,
    GraphicPresent
};

class Queue final : public WrappedObject<Queue, VkQueue>
{
  public:
    Queue(uint32_t queueFamilyIndex);

    Result init(Context* context);

    void terminate(VkDevice device);

    Result submitUpload(Context* context);

    Result submitActive(Context* context, std::vector<VkSemaphore>* waitSemaphores,
                        std::vector<VkSemaphore>* signalSemaphores);

    Result submit(CommandBuffer* commandBuffer, std::vector<VkSemaphore>* waitSemaphores = nullptr,
                  std::vector<VkSemaphore>* signalSemaphores = nullptr);

    inline CommandPool* getCommandPool()
    {
        ASSERT(commandPool);
        return commandPool;
    }

    VkResult present(const VkPresentInfoKHR& presentInfo);

    VkResult waitIdle();

  private:
    void getDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex);

    VkResult submit(const VkSubmitInfo& submitInfo, VkFence fence);

  private:
    CommandPool* commandPool;
    uint32_t queueFamilyIndex;
};

class QueueMap
{
  public:
    QueueMap();

    Result createQueueCreateInfos(PhysicalDevice* physicalDevice, Surface* surface);

    Result initQueues(Context* context);

    void terminate(VkDevice device);

    const std::vector<VkDeviceQueueCreateInfo>& getQueueCreateInfo() const;

  public:
    Queue* getQueue(QueueType type);

    CommandBuffer* getActiveCommandBuffer(const Context* context, QueueType type);

    CommandBuffer* getUploadCommandBuffer(const Context* context, QueueType type);

    void waitAll();

    inline const std::vector<uint32_t>& getQueueFamilyIndices()
    {
        return queueFamilyIndices;
    }

  private:
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::vector<uint32_t> queueFamilyIndices;
    float queuePriority = 1.0f;
    Queue* graphicsQueue;
    Queue* computeQueue;

    SerialIndex mCurrentQueueSerialIndex;
    Serial mCurrentSerial;
    Serial mLastFlushedSerial;
    Serial mLastSubmittedSerial;
};
} // namespace vk