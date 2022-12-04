#pragma once

#include "common/util.h"
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_core.h"

namespace vk
{
namespace handle
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

/*
====== DECLARE ======
*/
class Instance final : public WrappedObject<Instance, VkInstance>
{
  public:
    Instance() = default;
    void destroy();

    VkResult init(const VkInstanceCreateInfo& createInfo);
};

class PhysicalDevice final : public WrappedObject<PhysicalDevice, VkPhysicalDevice>
{
  public:
    void getProperties2(VkPhysicalDeviceProperties2* properties);
};

class Device final : public WrappedObject<Device, VkDevice>
{
  public:
    Device() = default;
    void destroy();

    VkResult init(VkPhysicalDevice device, const VkDeviceCreateInfo& createInfo);
};

/*
====== DEFINITION ======
*/
inline void Instance::destroy()
{
    if (valid())
    {
        vkDestroyInstance(mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

inline VkResult Instance::init(const VkInstanceCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateInstance(&createInfo, nullptr, &mHandle);
}

inline void PhysicalDevice::getProperties2(VkPhysicalDeviceProperties2* properties)
{
    ASSERT(!valid());

    vkGetPhysicalDeviceProperties2KHR(mHandle, properties);
}

inline void Device::destroy()
{
    if (valid())
    {
        vkDestroyDevice(mHandle, nullptr);
        mHandle = VK_NULL_HANDLE;
    }
}

inline VkResult Device::init(VkPhysicalDevice device, const VkDeviceCreateInfo& createInfo)
{
    ASSERT(!valid());
    return vkCreateDevice(device, &createInfo, nullptr, &mHandle);
}
} // namespace handle
} // namespace vk