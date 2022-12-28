#pragma once

#include "vulkan/core.h"

namespace vk
{
class Surface;
class PhysicalDevice;

enum class QueueType
{
    ComputeTransfer,
    GraphicPresent
};

class Queue final : public WrappedObject<Queue, VkQueue>
{
  public:
    Queue() = default;
    void terminate();

    void getDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex);
    VkResult submit(const VkSubmitInfo& submitInfo, VkFence fence);
    VkResult waitIdle();
    VkResult present(const VkPresentInfoKHR& presentInfo);
};

class QueueMap
{
  public:
    Result createQueueCreateInfos(PhysicalDevice* physicalDevice, Surface* surface);
    const std::vector<VkDeviceQueueCreateInfo>& getQueueCreateInfo() const
    {
        return queueCreateInfos;
    }

  private:
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.0f;
};
} // namespace vk