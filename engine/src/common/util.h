#pragma once

#include "external.h"
#include "log.h"
#include <stdint.h>
#include <vector>

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

#define DELETE(obj)                                                                                                    \
    if (obj)                                                                                                           \
    {                                                                                                                  \
        delete obj;                                                                                                    \
        obj = nullptr;                                                                                                 \
    }

#define TERMINATE(obj, ...)                                                                                            \
    if (obj)                                                                                                           \
    {                                                                                                                  \
        obj->terminate(__VA_ARGS__);                                                                                   \
        delete obj;                                                                                                    \
        obj = nullptr;                                                                                                 \
    }

#define RELEASE(obj) obj = nullptr;

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

#define try_call(expr)                                                                                                 \
    if ((expr) == Result::Fail)                                                                                        \
    {                                                                                                                  \
        LOGE("Failed to call: {" #expr "}");                                                                           \
    }