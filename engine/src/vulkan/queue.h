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

  private:
    void getDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex);

    VkResult submit(const VkSubmitInfo& submitInfo, VkFence fence);

    VkResult waitIdle();

    VkResult present(const VkPresentInfoKHR& presentInfo);

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

    Queue* getQueue();

  private:
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.0f;
    Queue* graphicsQueue;
    Queue* computeQueue;

    SerialIndex mCurrentQueueSerialIndex;
    Serial mCurrentSerial;
    Serial mLastFlushedSerial;
    Serial mLastSubmittedSerial;
};
} // namespace vk