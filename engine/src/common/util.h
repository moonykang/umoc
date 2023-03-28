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

#if defined(_MSC_VER)
#define ALIGNED(x) __declspec(align(x))
#else
#if defined(__GNUC__) || defined(__clang__)
#define ALIGNED(x) __attribute__((aligned(x)))
#endif
#endif