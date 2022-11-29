#pragma once

#include "common/util.h"
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_core.h"

#define VKCALL(expr)                                                                                                   \
    if ((expr) != VK_SUCCESS)                                                                                          \
    {                                                                                                                  \
        ASSERT(true)                                                                                                   \
    }