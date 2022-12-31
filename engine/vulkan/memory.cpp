#include "vulkan/memory.h"
#include "vulkan/context.h"
#include "vulkan/device.h"
#include "vulkan/physical_device.h"

namespace vk
{
DeviceMemory::DeviceMemory() : memoryProperty()
{
}

Result DeviceMemory::init(Context* context, const VkMemoryRequirements& memoryRequirements,
                          VkMemoryPropertyFlags memoryProperty)
{
    Device* device = context->getDevice();
    PhysicalDevice* physicalDevice = context->getPhysicalDevice();

    this->memoryProperty = memoryProperty;

    uint32_t memoryTypeIndex =
        physicalDevice->getPhysicalDeviceMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryProperty);

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = nullptr;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

    vk_try(allocate(device->getHandle(), memoryAllocateInfo));

    return Result::Continue;
}

void DeviceMemory::terminate(VkDevice device)
{
    if (valid())
    {
        vkFreeMemory(device, mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

VkResult DeviceMemory::allocate(VkDevice device, const VkMemoryAllocateInfo& allocateInfo)
{
    ASSERT(!valid());
    return vkAllocateMemory(device, &allocateInfo, nullptr, &mHandle);
}
} // namespace vk