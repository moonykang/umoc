#pragma once

#include "common/util.h"
#include "vulkan/debug.h"
#include "vulkan/wrapper.h"

#define vk_try(expr)                                                                                                   \
    {                                                                                                                  \
        VkResult result = (expr);                                                                                      \
        if (result != VK_SUCCESS)                                                                                      \
        {                                                                                                              \
            LOGD("VkResult %s for [" #expr "]", vk::debug::getVkResultString(result).c_str());                         \
            ASSERT(true);                                                                                              \
            return Result::Fail;                                                                                       \
        }                                                                                                              \
    }

#define vk_call(expr)                                                                                                  \
    {                                                                                                                  \
        VkResult result = (expr);                                                                                      \
        if (result != VK_SUCCESS)                                                                                      \
        {                                                                                                              \
            LOGD("VkResult %s for [" #expr "]", vk::debug::getVkResultString(result).c_str());                         \
            ASSERT(true);                                                                                              \
        }                                                                                                              \
    }