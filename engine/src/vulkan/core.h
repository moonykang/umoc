#pragma once

#include "common/serial.h"
#include "common/util.h"
#include "vulkan/debug.h"
#include "vulkan/extension.h"

namespace vk
{
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

#define call(expr)                                                                                                     \
    {                                                                                                                  \
        VkResult result = (expr);                                                                                      \
        if (result != VK_SUCCESS)                                                                                      \
        {                                                                                                              \
            LOGD("VkResult %s for [" #expr "]", vk::debug::getVkResultString(result).c_str());                         \
            ASSERT(true);                                                                                              \
        }                                                                                                              \
    }

} // namespace vk
