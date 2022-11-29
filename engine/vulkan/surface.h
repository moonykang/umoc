#pragma once

#include "vulkan/core.h"

namespace vk
{
class Surface
{
  public:
    VkSurfaceKHR& getSurface();

  protected:
    VkSurfaceKHR surface;
};
} // namespace vk
