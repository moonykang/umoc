#pragma once

#include "vulkan/core.h"

namespace vk
{
class Context;

class DeviceMemory final : public WrappedObject<DeviceMemory, VkDeviceMemory>
{
  public:
    DeviceMemory();

    ~DeviceMemory() = default;

    Result init(Context* context, const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags memoryProperty);

    void terminate(Context* context);

    Result map(Context* context, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData);

    void unmap(Context* context);

  private:
    VkResult allocate(VkDevice device, const VkMemoryAllocateInfo& allocateInfo);

  private:
    VkMemoryPropertyFlags memoryProperty;
};
} // namespace vk