#include "vulkan/queue.h"
#include "vulkan/physical_device.h"
#include "vulkan/surface.h"

namespace vk
{
void Queue::getDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex)
{
    ASSERT(!valid());
    vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &mHandle);
}

VkResult Queue::submit(const VkSubmitInfo& submitInfo, VkFence fence)
{
    ASSERT(!valid());
    return vkQueueSubmit(mHandle, 1, &submitInfo, fence);
}

VkResult Queue::waitIdle()
{
    ASSERT(!valid());
    return vkQueueWaitIdle(mHandle);
}

VkResult Queue::present(const VkPresentInfoKHR& presentInfo)
{
    ASSERT(!valid());
    return vkQueuePresentKHR(mHandle, &presentInfo);
}

/*
Graphics/Present
Compute/Transfer
*/
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

    if (computeTransferDedicatedQueueFlags != -1)
    {
        queueCreateInfo.queueFamilyIndex = computeTransferDedicatedQueueFamilyIndex;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    return Result::Continue;
}
} // namespace vk