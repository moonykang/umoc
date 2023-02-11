#pragma once

#include "rhi/buffer.h"
#include "vulkan/core.h"

namespace vk
{
class Context;
class DeviceMemory;

// Reference of actual buffer
class Buffer : public WrappedObject<Buffer, VkBuffer>
{
  public:
    Buffer(const VkBufferUsageFlags usage, const VkMemoryPropertyFlags memoryProperty,
           const VkMemoryAllocateFlags allocateFlags, const size_t size, const size_t offset = 0);

    virtual Result init(Context* context);

    virtual void terminate(VkDevice device);

  private:
    VkResult create(VkDevice device, const VkBufferCreateInfo& createInfo);

    const VkMemoryRequirements getMemoryRequirements(VkDevice device);

  private:
    DeviceMemory* deviceMemory;
    VkDeviceSize offset;
    VkDeviceSize size;

    VkBufferUsageFlags usage;
    VkMemoryPropertyFlags memoryProperty;
    VkMemoryAllocateFlags allocateFlags;
};

// Actual buffer
class ScratchBuffer : public Buffer
{
  public:
    ScratchBuffer(const VkBufferUsageFlags usage, const VkMemoryPropertyFlags memoryProperty,
                  const VkMemoryAllocateFlags allocateFlags, const size_t size, const size_t offset = 0);

    ~ScratchBuffer() = default;

  private:
};

class BufferManager
{
  public:
    BufferManager();

    ~BufferManager() = default;

    Result init(Context* context);

    void terminate(VkDevice device);

    // rhi::Buffer* createBuffer(Context* context);

  private:
    ScratchBuffer* vertexScratchBuffer;
};
} // namespace vk