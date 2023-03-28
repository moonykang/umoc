#pragma once

#include "common/serial.h"
#include "common/util.h"
#include "util.h"
#include "vulkan/extension.h"

namespace vk
{

namespace debug
{
VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                           VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                           const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                           void* pUserData);

std::string getVkResultString(VkResult result);
} // namespace debug

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

template <typename DerivedT, typename HandleT> class WrappedObjectList : NonCopyable
{
  public:
    bool valid() const
    {
        return !mList.empty();
    }

    const size_t size()
    {
        return mList.size();
    }

    const HandleT* data() const
    {
        return mList.data();
    }

    void put(HandleT handle)
    {
        mList.push_back(handle);
    }

  protected:
    WrappedObjectList() = default;

    ~WrappedObjectList() = default;

    WrappedObjectList(WrappedObjectList&& other)
    {
        mList = std::move(other.mList);
    }

    WrappedObjectList& operator=(WrappedObjectList&& other)
    {
        ASSERT(!valid());
        mList = std::move(other.mList);
        return *this;
    }

    std::vector<HandleT> mList;
};

#define vk_try(expr)                                                                                                   \
    {                                                                                                                  \
        VkResult result = (expr);                                                                                      \
        if (result != VK_SUCCESS)                                                                                      \
        {                                                                                                              \
            LOGE("VkResult %s for [" #expr "]", vk::debug::getVkResultString(result).c_str());                         \
            print_trace();                                                                                             \
            return Result::Fail;                                                                                       \
        }                                                                                                              \
    }

#define call(expr)                                                                                                     \
    {                                                                                                                  \
        VkResult result = (expr);                                                                                      \
        if (result != VK_SUCCESS)                                                                                      \
        {                                                                                                              \
            LOGE("VkResult %s for [" #expr "]", vk::debug::getVkResultString(result).c_str());                         \
            print_trace();                                                                                             \
        }                                                                                                              \
    }

} // namespace vk
