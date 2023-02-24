#pragma once

#include "common/serial.h"
#include "common/util.h"
#include "util.h"
#include "vulkan/debug.h"
#include "vulkan/extension.h"

namespace vk
{
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
