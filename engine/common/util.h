#pragma once

#include "external.h"
#include "log.h"

class NonCopyable
{
  protected:
    constexpr NonCopyable() = default;
    ~NonCopyable() = default;

  private:
    NonCopyable(const NonCopyable&) = delete;
    void operator=(const NonCopyable&) = delete;
};

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

#define DELETE(obj, ...)                                                                                               \
    if (obj)                                                                                                           \
    {                                                                                                                  \
        obj->terminate(__VA_ARGS__);                                                                                   \
        delete obj;                                                                                                    \
        obj = nullptr;                                                                                                 \
    }

enum class Result
{
    Continue,
    Fail,
};

#define try(expr)                                                                                                      \
    if ((expr) == Result::Fail)                                                                                        \
    {                                                                                                                  \
        LOGE("Failed to call: {" #expr "}");                                                                           \
        return Result::Fail;                                                                                           \
    }