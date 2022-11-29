#pragma once

#include "vulkan/core.h"

namespace platform
{
class Window;
}

namespace vk
{
class Surface;

class Window
{
  public:
    static platform::Window* createWindow();

    virtual void initSurface(VkInstance instance, Surface* surface) = 0;
};
} // namespace vk