#pragma once

#include "common/util.h"
#include "vulkan/extension.h"

namespace vk
{
template <typename DerivedT, typename HandleT> class WrappedObject : NonCopyable
{
  public:
    HandleT getHandle() const
    {
        return mHandle;
    }
    void setHandle(HandleT handle)
    {
        mHandle = handle;
    }
    bool valid() const
    {
        return mHandle != VK_NULL_HANDLE;
    }

    const HandleT* ptr() const
    {
        return &mHandle;
    }

    HandleT release()
    {
        HandleT handle = mHandle;
        mHandle = VK_NULL_HANDLE;
        return handle;
    }

  protected:
    WrappedObject() : mHandle(VK_NULL_HANDLE)
    {
    }
    ~WrappedObject()
    {
        ASSERT(!valid());
    }

    WrappedObject(WrappedObject&& other) : mHandle(other.mHandle)
    {
        other.mHandle = VK_NULL_HANDLE;
    }

    WrappedObject& operator=(WrappedObject&& other)
    {
        ASSERT(!valid());
        std::swap(mHandle, other.mHandle);
        return *this;
    }

    HandleT mHandle;
};

namespace handle
{
/*
====== DECLARE ======
*/
class CommandBuffer final : public WrappedObject<CommandBuffer, VkCommandBuffer>
{
  public:
    CommandBuffer() = default;

    void free(VkDevice device, VkCommandPool commandPool);
    VkResult allocate(VkDevice device, const VkCommandBufferAllocateInfo& allocateInfo);
    VkResult reset();
    VkResult begin(const VkCommandBufferBeginInfo& beginInfo);
    VkResult end();
};
/*
====== DEFINITION ======
*/

inline void CommandBuffer::free(VkDevice device, VkCommandPool commandPool)
{
    if (valid())
    {
        vkFreeCommandBuffers(device, commandPool, 1, &mHandle);
        mHandle = VK_NULL_HANDLE;
    }
}

inline VkResult CommandBuffer::allocate(VkDevice device, const VkCommandBufferAllocateInfo& allocateInfo)
{
    ASSERT(!valid());
    return vkAllocateCommandBuffers(device, &allocateInfo, &mHandle);
}

inline VkResult CommandBuffer::reset()
{
    ASSERT(valid());
    return vkResetCommandBuffer(mHandle, 0);
}

inline VkResult CommandBuffer::begin(const VkCommandBufferBeginInfo& info)
{
    ASSERT(valid());
    return vkBeginCommandBuffer(mHandle, &info);
}

inline VkResult CommandBuffer::end()
{
    ASSERT(valid());
    return vkEndCommandBuffer(mHandle);
}
} // namespace handle
} // namespace vk