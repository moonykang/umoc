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

#define DELETE(obj, dependency)                                                                                        \
    if (obj)                                                                                                           \
    {                                                                                                                  \
        obj->terminate(dependency);                                                                                    \
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